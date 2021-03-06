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
//	Main socket manipulation code.
//
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

bool
SocketManagerSymbol::getPort(DagNode* portArg, int& port)
{
  return succSymbol->getSignedInt(portArg, port) && port <= 65535;  // HACK
}

bool
SocketManagerSymbol::getActiveSocket(DagNode* socketArg, int& socketId)
{
  if (socketArg->symbol() == socketOidSymbol)
    {
      DagNode* idArg = safeCast(FreeDagNode*, socketArg)->getArgument(0);
      if (succSymbol->getSignedInt(idArg, socketId) &&
	  activeSockets.find(socketId) != activeSockets.end())
	return true;
    }
  return false;
}

bool
SocketManagerSymbol::getText(DagNode* textArg, crope& text)
{
  if (textArg->symbol() == stringSymbol)
    {
      text = safeCast(StringDagNode*, textArg)->getValue();
      return true;
    }
  return false;
}

bool
SocketManagerSymbol::setNonblockingFlag(int fd, FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  //
  //	Set nonblocking flag for a nascent socket; since it is not yet an external object we
  //	can just close it and generate an error reply if things don't work out.
  //
  int flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    {
      const char* errText = strerror(errno);
      DebugAdvisory("unexpected fcntl() GETFL: " << errText);
      close(fd);
      errorReply(errText, message, context);
      return false;
    }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
      const char* errText = strerror(errno);
      DebugAdvisory("unexpected fcntl() GETFL: " << errText);
      close(fd);
      errorReply(errText, message, context);
      return false;
    }
  return true;
}

bool
SocketManagerSymbol::createClientTcpSocket(FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  Assert(message->getArgument(0)->symbol() == this, "misdirected message");
  int port;
  DagNode* addressArg = message->getArgument(2);

  if (getPort(message->getArgument(3), port) && addressArg->symbol() == stringSymbol)
    {
      //
      //	We accept the message.
      //

      //
      //	Look up the address.
      //
      const crope& address = safeCast(StringDagNode*, addressArg)->getValue();
      hostent* record = gethostbyname(address.c_str());  // HACK - might block
      if (record == 0)
	{
	  DebugAdvisory("unexpected gethostbyname(() error: " << strerror(errno));
	  errorReply("bad address", message, context);
	  return true;
	}
      //
      //	Create a socket.
      //
      int fd = socket(PF_INET, SOCK_STREAM, 0);
      if (fd == -1)
	{
	  const char* errText = strerror(errno);
	  DebugAdvisory("unexpected socket() error: " << errText);
	  errorReply(errText, message, context);
	  return true;
	}
      //
      //	Make it non-blocking.
      //
      if (!setNonblockingFlag(fd, message, context))
	return true;
      //
      //	Try to connect to host.
      //
      sockaddr_in sockName;
      sockName.sin_family = AF_INET;
      sockName.sin_port = htons(port);
      sockName.sin_addr = *(reinterpret_cast<in_addr*>(record->h_addr_list[0]));  // HACK
      if (connect(fd, reinterpret_cast<sockaddr*>(&sockName), sizeof(sockName)) == 0)
	createdSocketReply(fd, message, context);  // instant success
      else if (errno == EINPROGRESS)
	{
	  //
	  //	Incomplete transaction on an asynchronous socket, so save details
	  //	so we know what to do when transaction completes.
	  //
	  ActiveSocket& as = activeSockets[fd];
	  as.state = WAITING_TO_CONNECT;
	  as.lastMessage.setNode(message);
	  as.originalContext = &context;
	  //
	  //	Completion (could be success or failure) is indicated by the operation system
	  //	making the socket writable.
	  //
	  wantTo(WRITE, fd);
	}
      else
	{
	  //
	  //	Connect failed, so report error and pretend that socket never existed.
	  //
	  DebugAdvisory("unexpected connect() error: " << strerror(errno));
	  close(fd);
	  errorReply("failed to connect", message, context);
	}
      return true;
    }
  IssueAdvisory("socket manager declined malformed message " << QUOTE(message) << '.');
  return false;
}

bool
SocketManagerSymbol::createServerTcpSocket(FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  Assert(message->getArgument(0)->symbol() == this, "misdirected message");
  int port;
  int backlog;

  if (getPort(message->getArgument(2), port) &&
      succSymbol->getSignedInt(message->getArgument(3), backlog) &&
      backlog > 0)
    {
      //
      //	Create a socket.
      //
      int fd = socket(PF_INET, SOCK_STREAM, 0);
      if (fd == -1)
	{
	  const char* errText = strerror(errno);
	  DebugAdvisory("unexpected socket() error: " << errText);
	  errorReply(errText, message, context);
	  return true;
	}
      //
      //	Make it non-blocking.
      //
      if (!setNonblockingFlag(fd, message, context))
	return true;
      {
	//
	//	Set SO_REUSEADDR so port can be immediately reused following the close()
	//	of this socket.
	//
	int value = 1;
	if (setsockopt(fd, SOL_SOCKET,  SO_REUSEADDR,  &value, sizeof(value)) == -1)
	  {
	    const char* errText = strerror(errno);
	    DebugAdvisory("setsockopt(SO_REUSEADDR) failed: " << errText);
	    errorReply(errText, message, context);
	    close(fd);
	    return true;
	  }
      }
      {
	//
	//	Bind it to the local port.
	//
	sockaddr_in sockName;
	sockName.sin_family = AF_INET;
	sockName.sin_port = htons(port);
	sockName.sin_addr.s_addr = htonl(INADDR_ANY);  // HACK - what is the portable way to set this?
	if (bind(fd, reinterpret_cast<sockaddr*>(&sockName), sizeof(sockName)) == -1)
	  {
	    const char* errText = strerror(errno);
	    DebugAdvisory("unexpected bind() error with fd " << fd << ": " << errText);
	    errorReply(errText, message, context);
	    close(fd);
	    return true;
	  }
      }
      //
      //	Start listening for connections.
      //
      if (listen(fd, backlog) == -1)
	{
	  const char* errText = strerror(errno);
	  DebugAdvisory("unexpected listen() error: " << errText);
	  errorReply(errText, message, context);
	  close(fd);
	  return true;
	}
      //
      //	Return a message now that we have a bound and listening server socket.
      //
      createdSocketReply(fd, message, context);
      activeSockets[fd].state = LISTENING;  // HACK - already set to nominal
     return true;
    }
  IssueAdvisory("socket manager declined malformed message " << QUOTE(message) << '.');
  return false;
}

bool
SocketManagerSymbol::acceptClient(FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  int socketId;
  DagNode* socketName = message->getArgument(0);
  if (getActiveSocket(socketName, socketId))
    {
      ActiveSocket& as = activeSockets[socketId];
      if (as.state == LISTENING)
	{
	  sockaddr_in sockName;
	  socklen_t addrLen = sizeof(sockName);
	  int r;
	  do
	    r = accept(socketId, reinterpret_cast<sockaddr*>(&sockName), &addrLen);
	  while (r == -1 && errno == EINTR);
	  if (r >= 0)
	    {
	      if (setNonblockingFlag(r, message, context))
		acceptedClientReply(inet_ntoa(sockName.sin_addr), r, message, context);
	    }
	  else if (errno == EAGAIN)
	    {
	      as.state = WAITING_TO_ACCEPT;
	      as.lastMessage.setNode(message);
	      as.originalContext = &context;
	      wantTo(READ, socketId);
	    }
	  else
	    {
	      //
	      //	What should we do with a socket that we failed to accept on?
	      //
	      const char* errText = strerror(errno);
	      DebugAdvisory("unexpected accept() error: " << errText);
	      errorReply(errText, message, context);
	    }
	  return true;
	}
      IssueAdvisory(socketName << " declined message " << QUOTE(message) << '.');
      return false;
    }
  IssueAdvisory("no socket to receive message " << QUOTE(message) << '.');
  return false;
}

bool
SocketManagerSymbol::send(FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  int socketId;
  crope text;
  DagNode* socketName = message->getArgument(0);
  if (getActiveSocket(socketName, socketId) &&
      getText(message->getArgument(2), text) &&
      text.size() != 0)
    {
      ActiveSocket& as = activeSockets[socketId];
      if ((as.state & ~WAITING_TO_READ) == 0)
	{
	  as.text = text;
	  as.unsent = text.c_str();
	  as.nrUnsent = text.size();  // how to deal with empty message?

	  ssize_t n;
	  do
	    n = write(socketId, as.unsent, as.nrUnsent);
	  while (n == -1 && errno == EINTR);

	  if (n == -1 && errno == EAGAIN)
	    n = 0;
	  if (n >= 0)
	    {
	      as.nrUnsent -= n;
	      if (as.nrUnsent == 0)
		{
		  sentMsgReply(message, context);
		  // clear  as.text
		}
	      else
		{
		  as.state |= WAITING_TO_WRITE;
		  as.lastMessage.setNode(message);
		  as.originalContext = &context;
		  as.unsent += n;
		  wantTo(WRITE, socketId);
		}
	    }
	  else
	    {
	      const char* errText = strerror(errno);
	      DebugAdvisory("unexpected write() error : " << errText);
	      closedSocketReply(socketId, errText, message, context);
	    }
	  return true;
	}
      IssueAdvisory(socketName << " declined message " << QUOTE(message) << '.');
      return false;
    }
  IssueAdvisory("no socket to receive message " << QUOTE(message) << '.');
  return false;
}

bool
SocketManagerSymbol::receive(FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  DagNode* socketName = message->getArgument(0);
  int socketId;
  if (getActiveSocket(socketName, socketId))
    {
      ActiveSocket& as = activeSockets[socketId];
      if ((as.state & ~WAITING_TO_WRITE) == 0)
	{
	  char buffer[READ_BUFFER_SIZE];
	  ssize_t n;
	  do
	    n = read(socketId, buffer, READ_BUFFER_SIZE);
	  while (n == -1 && errno == EINTR);

	  if (n > 0)
	    receivedMsgReply(buffer, n, message, context);
	  else
	    {
	      if (n == -1)
		{
		  if (errno == EAGAIN)
		    {
		      as.state |= WAITING_TO_READ;
		      as.lastMessage.setNode(message);
		      as.originalContext = &context;
		      wantTo(READ, socketId);
		    }
		  else
		    {
		      const char* errText = strerror(errno);
		      DebugAdvisory("unexpected read() error: " << errText);
		      closedSocketReply(socketId, errText, message, context);
		    }
		}
	      else
		{
		  DebugAdvisory("read 0 bytes");
		  closedSocketReply(socketId, "", message, context);
		}
	    }
	  return true;
	}
      else
	DebugAdvisory("as.state = " << as.state);
      IssueAdvisory(socketName << " declined message " << QUOTE(message) << '.');
      return false;
    }
  IssueAdvisory("no socket to receive message " << QUOTE(message) << '.');
  return false;
}

bool
SocketManagerSymbol::closeSocket(FreeDagNode* message, ObjectSystemRewritingContext& context)
{
  DagNode* socketName = message->getArgument(0);
  int socketId;
  if (getActiveSocket(socketName, socketId))
    {
      closedSocketReply(socketId, "", message, context);
      return true;
    }
  IssueAdvisory("no socket to receive message " << QUOTE(message) << '.');
  return false;
}

void
SocketManagerSymbol::cleanUp(DagNode* objectId)
{
  int socketId;
  if (getActiveSocket(objectId, socketId))
    {
      DebugAdvisory("cleaning up " << objectId);
      close(socketId);
      activeSockets.erase(socketId);
      PseudoThread::clearFlags(socketId);  // to avoid eventLoop() testing an invalid fd
    }
  else
    CantHappen("no socket for " << objectId);
}
