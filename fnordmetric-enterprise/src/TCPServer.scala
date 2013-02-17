// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import java.net.InetSocketAddress
import java.util.concurrent._
import org.jboss.netty.bootstrap.ServerBootstrap
import org.jboss.netty.channel.socket.nio.NioServerSocketChannelFactory
import org.jboss.netty.channel.Channels
import org.jboss.netty.channel.ChannelPipeline
import org.jboss.netty.channel.ChannelPipelineFactory
import org.jboss.netty.handler.codec.frame.DelimiterBasedFrameDecoder
import org.jboss.netty.handler.codec.frame.Delimiters
import org.jboss.netty.handler.codec.string.StringDecoder
import org.jboss.netty.handler.codec.string.StringEncoder

class TCPServer(port: Int, threads: Int) {

  val MAX_FRAME_LENGTH = 8192

  val bootstrap = new ServerBootstrap(
    new NioServerSocketChannelFactory(Executors.newCachedThreadPool, Executors.newCachedThreadPool))

  bootstrap.setPipelineFactory(new TCPServerPipelineFactory)
  bootstrap.bind(new InetSocketAddress(port))

  class TCPServerPipelineFactory extends ChannelPipelineFactory {

    override def getPipeline : ChannelPipeline = {
      val pipeline = Channels.pipeline

      pipeline.addLast("framer", new DelimiterBasedFrameDecoder(MAX_FRAME_LENGTH,
        (Delimiters.lineDelimiter): _*))

      pipeline.addLast("decoder", new StringDecoder)
      pipeline.addLast("eccoder", new StringEncoder)
      pipeline.addLast("handler", new TCPHandler)

      pipeline
    }

  }

}
