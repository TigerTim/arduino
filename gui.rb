#!/usr/bin/env ruby
=begin
  helloworld.rb - Ruby/GTK first sample script.

  Copyright (c) 2002,2003 Ruby-GNOME2 Project Team
  This program is licenced under the same licence as Ruby-GNOME2.

  $Id: helloworld.rb,v 1.4 2003/02/01 16:46:22 mutoh Exp $
=end

require 'gtk2'

require 'serialport'

def save

    title = @txTitle.text.chomp
    ventilopen1 = @txVentilOpen1.text.chomp.to_i
    ventilwait = @txWaitVentilOpen.text.chomp.to_i
    ventilopen2 = @txVentilOpen2.text.chomp.to_i
    waitshot = @txDelaytime.text.chomp.to_i
    waitBlitz = @txBlitzDelaytime.text.chomp.to_i

    save_file = File.new("setup.save","a")
    save_file.puts "#{title},#{ventilopen1},#{ventilwait},#{ventilopen2},#{waitshot},#{waitBlitz}"
    save_file.close

end

def load(title)

  save_file = File.new("setup.save","r")
  while (line = save_file.gets)
    arr = line.chomp.split(",")
    puts arr.inspect
    if (arr[0] == title)
      @txVentilOpen1.text = arr[1]
      @txWaitVentilOpen.text = arr[2]
      @txVentilOpen2.text = arr[3]
      @txDelaytime.text = arr[4]
      @txBlitzDelaytime.text = arr[5]
    end
  end
  save_file.close

end


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
    waitBlitz = @txBlitzDelaytime.text.chomp.to_i

    @sp.print ventilopen1.to_s + ","
    @sp.print ventilwait.to_s + ","
    @sp.print ventilopen2.to_s + ","
    @sp.print "#{waitshot - ventilwait - ventilopen2 - ventilopen2},"
    @sp.print waitBlitz
    #sp.close                       #see note 1

end

def createUI

  layout = Gtk::Table.new(20,2)
  layout.column_spacings = 5
  layout.row_spacings = 5


  laVentilOpen1 = Gtk::Label.new("time ventil open")
  layout.attach_defaults(laVentilOpen1,0,1,0,1)

  @txVentilOpen1 = Gtk::SpinButton.new(0,500,1)
  @txVentilOpen1.value = 50.to_f
  layout.attach_defaults(@txVentilOpen1,1,2,0,1)


  laWaitVentilOpen = Gtk::Label.new("wait for second drop")
  layout.attach_defaults(laWaitVentilOpen,0,1,1,2)

  @txWaitVentilOpen = Gtk::SpinButton.new(0,500,1)
  @txWaitVentilOpen.value = 230
  layout.attach_defaults(@txWaitVentilOpen,1,2,1,2)


  laVentilOpen2 = Gtk::Label.new("time ventil open")
  layout.attach_defaults(laVentilOpen2,0,1,2,3)

  @txVentilOpen2 = Gtk::SpinButton.new(0,500,1)
  @txVentilOpen2.value = 50
  layout.attach_defaults(@txVentilOpen2,1,2,2,3)

  layout.set_row_spacing(2,20)

  laBlitzDelaytime = Gtk::Label.new("wait to blitz ")
  layout.attach_defaults(laBlitzDelaytime,0,1,3,4)

  @txBlitzDelaytime = Gtk::SpinButton.new(0,500,1)
  @txBlitzDelaytime.value = 420
  layout.attach_defaults(@txBlitzDelaytime,1,2,3,4)


  laDelaytime = Gtk::Label.new("wait to shot ")
  layout.attach_defaults(laDelaytime,0,1,4,5)

  @txDelaytime = Gtk::SpinButton.new(0,500,1)
  @txDelaytime.value = 420
  layout.attach_defaults(@txDelaytime,1,2,4,5)

  

  layout.set_row_spacing(4,30)
  btSend = Gtk::Button.new("Send")
  layout.attach_defaults(btSend,0,1,5,6)
  
  btSend.signal_connect("clicked") {
    send()
  }

  @txSerialPort = Gtk::Entry.new
  @txSerialPort.text = "COM4"
  layout.attach_defaults(@txSerialPort,1,2,5,6)

  btRead = Gtk::Button.new("Read")
  layout.attach_defaults(btRead,0,1,6,7)
  btRead.signal_connect("clicked") {
    i = 0
    t = Thread.new { 
          loop { 
            s = @sp.read
            @buf.insert(@buf.end_iter,"#{s}\n")
            @scroll.vadjustment.value = @scroll.vadjustment.upper - @scroll.vadjustment.page_size
            sleep(1)
          }
        }
  }

  btSave = Gtk::Button.new("Save")
  layout.attach_defaults(btSave,0,1,7,8)
  btSave.signal_connect("clicked") {
    save()
  }

  @txTitle = Gtk::Entry.new
  @txTitle.text = "noname"
  layout.attach_defaults(@txTitle,1,2,7,8)


  btLoad = Gtk::Button.new("Load")
  layout.attach_defaults(btLoad,0,1,8,9)
  btLoad.signal_connect("clicked") {
    load(@liTitle.active_text)
  }

  @liTitle = Gtk::ComboBox.new
  save_file = File.new("setup.save","r")
  while (line = save_file.gets)
    @liTitle.append_text("#{line.split(",")[0]}")
  end
  save_file.close
  layout.attach_defaults(@liTitle,1,2,8,9)

  @scroll = Gtk::ScrolledWindow.new
  @buf = Gtk::TextBuffer.new
  @buf.text = 'test'
  log = Gtk::TextView.new(@buf)

  @scroll.add(log)
  @scroll.set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS )
  @scroll.set_shadow_type(Gtk::SHADOW_ETCHED_IN)
  @scroll.set_size_request(200,500)
  layout.attach_defaults(@scroll,0,2,9,15)

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

#@sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

#puts @sp.read

createUI