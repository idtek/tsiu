proc build-tcp { type src dest pktSize window class startTime } {
   global ns

   if { $type == "TCP" } {
      set tcp [new Agent/TCP]
      set snk [new Agent/TCPSink]
   } elseif { $type == "Reno" } {
      set tcp [new Agent/TCP/Reno]
      set snk [new Agent/TCPSink]
   } elseif { $type == "Sack" } {
      set tcp [new Agent/TCP/Sack1]
      set snk [new Agent/TCPSink/Sack1]
   } elseif  { $type == "Newreno" } {
      set tcp [new Agent/TCP/Newreno]
      set snk [new Agent/TCPSink]
   } else {
      puts "ERROR: Inavlid tcp type"
   }
   $ns attach-agent $src $tcp

   #$tcp set tcpTick_ 0.01

   $ns attach-agent $dest $snk

   $ns connect $tcp $snk

   if { $pktSize > 0 } {
      $tcp set packetSize_ $pktSize
   }
   $tcp set class_ $class
   if { $window > 0 } {
      $tcp set window_ $window
   } else {
      # default in ns-2 version 2.0
      $tcp set window_ 20
   }

   set ftp [new Source/FTP]
   $ftp set agent_ $tcp
   $ns at $startTime "$ftp start"

   return $tcp
}

proc build-cbr { src dest rate id startTime stopTime } {
   global ns

   set udp [new Agent/UDP]
   $ns attach-agent $src $udp

   set cbr [new Application/Traffic/CBR]
   $cbr attach-agent $udp

   set null [new Agent/Null]
   $ns attach-agent $dest $null

   $ns connect $udp $null

   $cbr set rate_ $rate

   $ns at $startTime "$cbr start"
   $ns at $stopTime "$cbr stop"

   return $cbr
}

proc build-on-off { src dest pktSize burstTime idleTime rate id startTime } {
   global ns

   set udp [new Agent/CBR/UDP]
   $ns attach-agent $src $udp

   set null [new Agent/Null]
   $ns attach-agent $dest $null

   $ns connect $udp $null

   set exp [new Traffic/Expoo]
   $exp set packet-size $pktSize
   $exp set burst-time $burstTime
   $exp set idle-time $idleTime
   $exp set rate $rate
   $udp attach-traffic $exp

   $ns at $startTime "$udp start"
   $udp set fid_ $id

   return $udp
}

proc build-udt { src dest mtu window id startTime } {
   global ns

   set udt0 [new Agent/UDT]
   set udt1 [new Agent/UDT]

   $udt0 set mtu_ $mtu
   $udt0 set max_flow_window_ $window

   $ns attach-agent $src $udt0
   $ns attach-agent $dest $udt1
   $ns connect $udt0 $udt1

   set ftp [new Application/FTP]
   $ftp attach-agent $udt0

   $udt0 set fid_ $id

   $ns at $startTime "$ftp start"

   return $udt0
}
