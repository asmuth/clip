import AssemblyKeys._

name := "FnordMetric Enterprise"

organization := "com.paulasmuth"

version := "0.0.1"

mainClass in (Compile, run) := Some("com.fnordmetric.enterprise.FnordMetric")

scalaSource in Compile <<= baseDirectory(_ / "src")

scalaSource in Test <<= baseDirectory(_ / "test")

scalaVersion := "2.9.1"

resolvers += "Couchbase Maven2 Repo" at "http://files.couchbase.com/maven2"

libraryDependencies += "org.eclipse.jetty" % "jetty-server" % "8.1.8.v20121106"

libraryDependencies += "org.eclipse.jetty" % "jetty-websocket" % "8.1.8.v20121106"

libraryDependencies += "org.scalatest" %% "scalatest" % "2.0.M1" % "test"

libraryDependencies += "io.netty" % "netty" % "3.6.0.Final"

assemblySettings

jarName in assembly <<= (version) { v => "FnordMetric-Enterprise-v" + v + ".jar" }

fork in run := true
