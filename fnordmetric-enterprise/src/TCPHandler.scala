// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import java.net.InetSocketAddress
import org.jboss.netty.buffer.ChannelBuffer
import org.jboss.netty.buffer.ChannelBuffers
import org.jboss.netty.channel._

class TCPHandler extends SimpleChannelUpstreamHandler {

  override def messageReceived(ctx: ChannelHandlerContext, e: MessageEvent) {
    val ins = InstructionFactory.parse(e.getMessage.toString)
    e.getChannel.write(ins.execute + "\n")
  }

  override def exceptionCaught(ctx: ChannelHandlerContext, e: ExceptionEvent) {
    FnordMetric.error("[TCP] Exception: " + e.getCause, false)
    e.getChannel.close()
  }

}
