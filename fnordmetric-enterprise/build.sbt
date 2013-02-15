import AssemblyKeys._

name := "FnordMetric Enterprise"

organization := "com.paulasmuth"

version := "0.0.1"

mainClass in (Compile, run) := Some("com.fnordmetric.enterprise.Enterprise")

scalaSource in Compile <<= baseDirectory(_ / "src")

scalaSource in Test <<= baseDirectory(_ / "test")

scalaVersion := "2.9.1"

resolvers += "Couchbase Maven2 Repo" at "http://files.couchbase.com/maven2"

libraryDependencies += "org.eclipse.jetty" % "jetty-server" % "7.2.2.v20101205"

libraryDependencies += "org.scalatest" %% "scalatest" % "2.0.M1" % "test"

assemblySettings

jarName in assembly <<= (version) { v => "FnordMetric-Enterprise-v" + v + ".jar" }

fork in run := true
