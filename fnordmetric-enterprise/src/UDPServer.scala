// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

import java.net.DatagramPacket
import java.net.DatagramSocket
import java.util.concurrent._

class UDPServer(port: Int, threads: Int){

  val pool = Executors.newFixedThreadPool(threads)
  val sock = new DatagramSocket(port)

  val buffer_size = 65535
  val buffer = new Array[Byte](buffer_size)
  val packet = new DatagramPacket(buffer, buffer_size)

  FnordMetric.log("Listening on udp://0.0.0.0:" + port)

  while (true) {
    sock.receive(packet)

    dispatch(
      new String(packet.getData, 0, packet.getLength))
  }

  private def dispatch(body: String) =
    pool.execute(new Runnable { def run : Unit = {
      for (msg <- body.split("\n")) execute(msg)
    }})

  private def execute(msg: String) =
    InstructionFactory.parse(msg) match {
      case e: ErrorInstruction =>
        FnordMetric.error("[UDP] " + e.execute, false)

      case e: SampleInstruction =>
        e.execute

      case e: AbstractInstruction =>
        FnordMetric.error("[UDP] received non-sample instruction", false)
    }

}
