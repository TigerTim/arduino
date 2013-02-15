#!/usr/bin/env ruby
=begin
  helloworld.rb - Ruby/GTK first sample script.

  Copyright (c) 2002,2003 Ruby-GNOME2 Project Team
  This program is licenced under the same licence as Ruby-GNOME2.

  $Id: helloworld.rb,v 1.4 2003/02/01 16:46:22 mutoh Exp $
=end

require 'gtk2'

require 'serialport'

def send

    #puts @txDelaytime.text

    #params for serial port
    
    #sp.close()
    #sp.open()
    #sleep(2)

    ventilopen1 = @txVentilOpen1.text.chomp.to_i
    ventilwait = @txWaitVentilOpen.text.chomp.to_i
    ventilopen2 = @txVentilOpen2.text.chomp.to_i
    waitshot = @txDelaytime.text.chomp.to_i

    @sp.print ventilopen1.to_s + ","
    @sp.print ventilwait.to_s + ","
    @sp.print ventilopen2.to_s + ","
    @sp.print "#{waitshot - ventilwait - ventilopen2}"
    #sp.close                       #see note 1

end

def createUI

  layout = Gtk::Table.new(3,2)
  layout.column_spacings = 5
  layout.row_spacings = 5


  laVentilOpen1 = Gtk::Label.new("time ventil open")
  layout.attach_defaults(laVentilOpen1,0,1,0,1)

  @txVentilOpen1 = Gtk::Entry.new
  @txVentilOpen1.text = '50'
  layout.attach_defaults(@txVentilOpen1,1,2,0,1)


  laWaitVentilOpen = Gtk::Label.new("wait for second drop")
  layout.attach_defaults(laWaitVentilOpen,0,1,1,2)

  @txWaitVentilOpen = Gtk::Entry.new
  @txWaitVentilOpen.text = '230'
  layout.attach_defaults(@txWaitVentilOpen,1,2,1,2)


  laVentilOpen2 = Gtk::Label.new("time ventil open")
  layout.attach_defaults(laVentilOpen2,0,1,2,3)

  @txVentilOpen2 = Gtk::Entry.new
  @txVentilOpen2.text = '50'
  layout.attach_defaults(@txVentilOpen2,1,2,2,3)

  layout.set_row_spacing(2,20)

  laDelaytime = Gtk::Label.new("wait to shot ")
  layout.attach_defaults(laDelaytime,0,1,3,4)

  @txDelaytime = Gtk::Entry.new
  @txDelaytime.text = '420'
  layout.attach_defaults(@txDelaytime,1,2,3,4)


  layout.set_row_spacing(3,30)
  btSend = Gtk::Button.new("Send")
  layout.attach_defaults(btSend,0,1,4,5)

  @txSerialPort = Gtk::Entry.new
  @txSerialPort.text = "COM4"
  layout.attach_defaults(@txSerialPort,1,2,4,5)

  
  btSend.signal_connect("clicked") {
    send()
  }

  btRead = Gtk::Button.new("Read")
  layout.attach_defaults(btRead,0,1,5,6)
  btRead.signal_connect("clicked") {
    t = Thread.new { 
          loop { 
            puts @sp.read
            sleep(1)
          }
        }
  }
  
  window = Gtk::Window.new
  window.border_width = 20

  window.signal_connect("delete_event") {
    puts "delete event occurred"
    #true
    false
  }

  window.signal_connect("destroy") {
    puts "destroy event occurred"
    Gtk.main_quit
  }  
  
  vbox = Gtk::VBox.new
  vbox.add(layout)
  window.add(vbox)

  window.show_all

  Gtk.main

end

port_str = 'COM4' #@txSerialPort.text  #may be different for you
baud_rate = 9600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE

@sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

puts @sp.read

createUI