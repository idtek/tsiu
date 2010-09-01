set ns [new Simulator]

Agent/UDT set mtu_ 1500
Agent/UDT set max_flow_window_ 32768

$ns color 1 Blue
$ns color 2 Red

set nf [open out.nam w]
$ns namtrace-all $nf

proc finish {} {
        global ns nf

        $ns flush-trace

        close $nf

        exec nam out.nam &
        exit 0
}

set n0 [$ns node]
set n1 [$ns node]

$ns duplex-link $n0 $n1 10Mb 5ms RED
$ns queue-limit $n0 $n1 10
$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n0 $n1 queuePos 0.5

set udt0 [new Agent/UDT]
set udt1 [new Agent/UDT]
$ns attach-agent $n0 $udt0
$ns attach-agent $n1 $udt1
$ns connect $udt0 $udt1

$udt0 set fid_ 1
$udt1 set fid_ 2

set ftp [new Application/FTP]
$ftp attach-agent $udt0

$ns at 0.05 "$ftp start"

$ns at 10.05 "finish"

$ns run
