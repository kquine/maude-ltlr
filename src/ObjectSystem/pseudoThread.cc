/*

    This file is part of the Maude 2 interpreter.

    Copyright 1997-2003 SRI International, Menlo Park, CA 94025, USA.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

//
//      Implementation for class PseudoThread.
//
#include <unistd.h>
#include <errno.h>

//      utility stuff
#include "macros.hh"
#include "pseudoThread.hh"

PseudoThread::FD_Info PseudoThread::fdInfo[MAX_NR_FDS];
int PseudoThread::firstActive = NONE;
PseudoThread::CallBackQueue PseudoThread::callBackQueue;

void
PseudoThread::requestCallBack(long notBefore)
{
  callBackQueue.push(CallBackRequest(this, notBefore));
}

long
PseudoThread::processCallBacks(int& returnValue)
{
  long now = getTime();
  do
    {
      CallBackRequest c = callBackQueue.top();  // deep copy
      long wait = c.notBefore - now;
      if (wait > 0)
	return wait;
      callBackQueue.pop();
      c.client->doCallBack();
      returnValue |= EVENT_HANDLED;
    }
  while (!(callBackQueue.empty()));
  return NONE;
}

int
PseudoThread::processFds(long wait)
{
  static pollfd ufds[MAX_NR_FDS];
  int nfds = 0;
  //	cerr << "firstActive is " << firstActive << endl;

  long now = getTime();
  for (int i = firstActive; i != NONE; i = fdInfo[i].nextActive)
    {
      Assert(fdInfo[i].flags != 0, "zero flags for " << i);
      ufds[nfds].fd = i;
      ufds[nfds].events = fdInfo[i].flags;
      if (fdInfo[i].timeOutAt != NONE)
	{
	  long delay = fdInfo[i].timeOutAt - now;
	  if (delay < 0)
	    delay = 0;
	  if (wait < 0 || delay < wait)
	    wait = delay;
	}
      ++nfds;
    }
  int milliseconds = -1;
  if (wait >= 0)
    {
      if (wait <= (INT_MAX / 1000))
	milliseconds = 1000 * wait;
      else
	milliseconds = INT_MAX;
    }
  int n = poll(ufds, nfds, milliseconds);
  //	cerr << "poll returns " << n << endl;
  if (n <= 0)
    {
      Assert(errno == EINTR, "poll() failed with " << errno);
      return INTERRUPTED;  // treat all errors as interrupts
    }
  //
  //	Dispatch pending events.
  //
  int returnValue = 0;
  for (int i = 0; i < nfds; i++)
    {
      //  cerr << "fd = " << ufds[i].fd << "  revents = " << ufds[i].revents << endl;
      int fd = ufds[i].fd;
      FD_Info& info = fdInfo[fd];
      if (short t = ufds[i].revents)
	{
	  Assert(!(t & POLLNVAL), "invalid fd " << fd);
	  //
	  //	When we have an error we need to removed fd from
	  //	active list to avoid going into a spin.
	  //
	  if (t & POLLERR)
	    {
	      info.flags = 0;
	      unlink(fd);
	      info.owner->doError(fd);
	      returnValue = EVENT_HANDLED;
	      continue;
	    }
	  //
	  //	This is tricky. If we are wanting to read but
	  //	get disconnected:
	  //	Linux sets POLLHUP but not POLLIN;
	  //	BSD & Solaris set POLLIN but not POLLHUP;
	  //	DEC UNIX sets both flags even if there are still
	  //	  chars to read.
	  //
	  if (t & POLLIN)
	    {
	      if (!(info.flags = t & ~POLLIN))
		unlink(fd);
	      info.timeOutAt = NONE;
	      info.owner->doRead(fd);
	      returnValue = EVENT_HANDLED;
	    }
	  else if (t & POLLHUP)
	    {
	      //
	      //	Need to removed fd from active list to avoid going
	      //	into a spin. But we don't want to do this until all
	      //	available chars have been read.
	      //
	      info.flags = 0;
	      unlink(fd);
	      info.owner->doHungUp(fd);
	      returnValue = EVENT_HANDLED;
	      continue;
	    }
	  if (t & POLLOUT)
	    {
	      if (!(info.flags = t & ~POLLOUT))
		unlink(fd);
	      info.timeOutAt = NONE;
	      info.owner->doWrite(fd);
	      returnValue = EVENT_HANDLED;
	    }
	}
      else
	{
	  //
	  //	Check for time out.
	  //
	  if (info.timeOutAt != NONE && info.timeOutAt < now)
	    {
	      info.flags = 0;
	      unlink(fd);
	      info.owner->doTimeOut(fd);
	      returnValue = EVENT_HANDLED;
	    }
	}
    }
  return returnValue;
}

int
PseudoThread::eventLoop()
{
  //
  //	We return for one or more of 3 reasons:
  //	NOTHING_PENDING: no clients waiting for callbacks
  //	INTERRUPTED: a non-ignored signal arrived
  //	EVENT_HANDLED: at least one callback was made
  //
  if (callBackQueue.empty())
    return firstActive == NONE ? NOTHING_PENDING : processFds(NONE);

  int returnValue = 0;
  for(;;)
    {
      long wait = processCallBacks(returnValue);
      if (firstActive == NONE)
	{
	  if (returnValue != 0)
	    break;
	  if (sleep(wait) != 0)
	    return INTERRUPTED;
	}
      else
	{
	  returnValue |= processFds(wait);
	  if (returnValue != 0)
	    break;
	}
    }
  return returnValue;
}

void
PseudoThread::clearFlags(int fd)
{
  if (fdInfo[fd].flags != 0)
    {
      fdInfo[fd].flags = 0;
      unlink(fd);
    }
}

void
PseudoThread::wantTo(int flags, int fd, long timeOutAt)
{
  Assert(flags != 0, "empty flags for " << fd);
  int t = fdInfo[fd].flags;
  if (t == 0)
    {
      link(fd);
      fdInfo[fd].owner = this;
    }
  else
    Assert(fdInfo[fd].owner == this, "owner mismatch for " << fd);
  fdInfo[fd].flags = t | flags;
  fdInfo[fd].timeOutAt = timeOutAt;
}

void
PseudoThread::doRead(int fd)
{
  Assert(false, "failed to do read on " << fd);
}

void
PseudoThread::doWrite(int fd)
{
  Assert(false, "failed to do write on " << fd);
}

void
PseudoThread::doError(int fd)
{
  Assert(false, "failed to do error on " << fd);
}

void
PseudoThread::doHungUp(int fd)
{
  Assert(false, "failed to do hung up on " << fd);
}

void
PseudoThread::doTimeOut(int fd)
{
  Assert(false, "failed to do time out on " << fd);
}

void
PseudoThread::doCallBack()
{
  Assert(false, "failed to do call back");
}

void
PseudoThread::link(int fd)
{
  fdInfo[fd].prevActive = NONE;
  fdInfo[fd].nextActive = firstActive;
  if (firstActive != NONE)
    fdInfo[firstActive].prevActive = fd;
  firstActive = fd;
}

void
PseudoThread::unlink(int fd)
{
  int prev = fdInfo[fd].prevActive;
  int next = fdInfo[fd].nextActive;
  if (next != NONE)
    fdInfo[next].prevActive = prev;
  if (prev != NONE)
    fdInfo[prev].nextActive = next;
  else
    firstActive = next;
}
