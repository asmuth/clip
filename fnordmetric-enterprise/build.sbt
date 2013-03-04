import AssemblyKeys._

name := "FnordMetric Enterprise"

organization := "com.paulasmuth"

version := "0.0.2"

mainClass in (Compile, run) := Some("com.fnordmetric.enterprise.FnordMetric")

scalaSource in Compile <<= baseDirectory(_ / "src")

scalaVersion := "2.9.1"

resolvers += "Couchbase Maven2 Repo" at "http://files.couchbase.com/maven2"

libraryDependencies ++= Seq(
  "io.netty"                % "netty"           % "3.6.0.Final",
  "org.eclipse.jetty"       % "jetty-websocket" % "8.1.8.v20121106",
  "org.eclipse.jetty"       % "jetty-server"    % "8.1.8.v20121106",
  "org.eclipse.jetty"       % "jetty-webapp"    % "8.1.7.v20120910"     % "compile",
  "org.eclipse.jetty.orbit" % "javax.servlet"   % "3.0.0.v201112011016" % "compile" artifacts Artifact("javax.servlet", "jar", "jar")
)

assemblySettings

jarName in assembly <<= (version) { v => "FnordMetric-Enterprise-v" + v + ".jar" }

mergeStrategy in assembly <<= (mergeStrategy in assembly) { (old) =>
  {
    case "about.html" => MergeStrategy.discard
    case x => old(x)
  }
}

fork in run := true
