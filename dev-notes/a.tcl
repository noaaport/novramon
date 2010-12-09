#!/usr/local/bin/tclsh8.6

set param_list [list carrier_frequency \
  symbol_rate \
  lnb_fault \
  signal_lock \
  data_lock \
  signal_strength_as_percentage \
  signal_strength_as_dbm \
  double carrier_to_noise \
  double vber \
  uncorrectables \
  frequency_offset \
  clock_offset \
  data_sync_loss \
  front_end_lock_loss \
  frequency_error \
  ethernet_transmit \
  ethernet_transmit_error \
  ethernet_receive \
  ethernet_receive_error \
  ethernet_packet_dropped \
  total_ethernet_packets_out \
  dvb_accepted \
  dvb_scrambled \
  total_dvb_packets_accepted \
  total_dvb_packets_rx_in_error \
  total_uncorrectable_ts_packets \
  nest \
  demodulator_gain \
  digital_gain \
  agc \
  agca \
  agcn \
  gnyqa \
  gfara];

set s {
if(r->hasParameter($P)){
  s->$p = r->getParameter($P).asLong();
}}
	
foreach p $param_list {
    set P [string toupper $p];
    puts [subst $s];
}
