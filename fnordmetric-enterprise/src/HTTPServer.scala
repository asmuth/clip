// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import org.eclipse.jetty.server.Server
import org.eclipse.jetty.server.handler.AbstractHandler
import org.eclipse.jetty.util.thread.QueuedThreadPool

import java.io.PrintStream
import java.io.OutputStream

class HTTPServer(port: Int, num_threads: Int, handler: AbstractHandler) {
  val server = new Server(port)
  /*server.setThreadPool(pool)
  server.setGracefulShutdown(1000)*/

  server.setHandler(handler)
  without_stderr(_ => server.start())

  FnordMetric.log("Listening on ws://0.0.0.0:" + port)

  def without_stderr(lambda: Unit => Unit) : Unit = {
    val stderr = System.err

    val dummy = new PrintStream(new OutputStream(){
      def write(b: Int) : Unit = ()
    })

    System.setErr(dummy)
    lambda()
    System.setErr(stderr)
  }

}
