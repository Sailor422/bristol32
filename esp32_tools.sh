#!/bin/bash
# ESP32 Development Helper Script for Bristol32 Project

echo "🔍 Detecting ESP32 devices..."
echo "Available serial ports:"
ls /dev/tty* | grep -E "(usb|modem)" || echo "No USB serial devices found"

echo ""
echo "📋 Available commands:"
echo "  pio run -e ext_mmwave        # Build external sensor node"
echo "  pio run -e int_gateway       # Build gateway node"
echo "  pio run -e ext_mmwave -t upload # Build and upload to external node"
echo "  pio run -e int_gateway -t upload # Build and upload to gateway node"
echo "  pio device list              # List connected devices"
echo "  screen /dev/tty.usbmodemXXXX 115200 # Open serial console"
echo ""
echo "⚡ Quick test commands:"
echo "  esptool chip_id              # Test ESP32 connection"
echo "  pio run -e ext_mmwave -t upload --upload-port /dev/tty.usbmodemXXXX"