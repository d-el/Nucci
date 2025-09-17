#!/usr/bin/env python3
# API and command line program
# version 0.3

import argparse
import enum
import time
from pathlib import Path

from pymodbus import (
    client
)
from pymodbus.pdu import FileRecord
from tqdm import tqdm


class Modbus:
    def __init__(self, ip, port=502):
        self.client = client.ModbusTcpClient(ip, port=port,
                                             timeout=3,
                                             retries=3,
                                             # retry_on_empty=True,
                                             reconnect_delay=1,
                                             reconnect_delay_max=9)
        print('Connected to {}:{}'.format(ip, port))

    def __del__(self):
        self.client.close()

    def write_u16(self, reg, val, slaveaddr):
        self.client.write_register(reg, val, slave=slaveaddr)

    def read_u16(self, reg, slaveaddr):
        r = self.client.read_holding_registers(reg, count=1, slave=slaveaddr)
        return r.registers[0]

    def read_i16(self, reg, slaveaddr):
        result = self.client.read_holding_registers(reg, count=1, slave=slaveaddr)
        return self.client.convert_from_registers(result.registers, data_type=self.client.DATATYPE.INT16,
                                                  word_order='little')

    def write_i16(self, reg, slaveaddr):
        pay = self.client.convert_to_registers(val, data_type=self.client.DATATYPE.INT16)
        self.client.write_registers(reg, pay, skip_encode=True, slave=slaveaddr)

    def write_u32(self, reg, val, slaveaddr):
        pay = self.client.convert_to_registers(val, data_type=self.client.DATATYPE.UINT32, word_order='little')
        self.client.write_registers(reg, pay, slave=slaveaddr)

    def read_u32(self, reg, slaveaddr):
        result = self.client.read_holding_registers(reg, count=2, slave=slaveaddr)
        return self.client.convert_from_registers(result.registers, data_type=self.client.DATATYPE.UINT32,
                                                  word_order='little')

    def write_i32(self, reg, val, slaveaddr):
        print(reg, val, slaveaddr)
        pay = self.client.convert_to_registers(val, data_type=self.client.DATATYPE.INT32, word_order='little')
        self.client.write_registers(reg, pay, slave=slaveaddr)

    def read_i32(self, reg, slaveaddr):
        result = self.client.read_holding_registers(reg, count=2, slave=slaveaddr)
        return self.client.convert_from_registers(result.registers, data_type=self.client.DATATYPE.INT32,
                                                  word_order='little')

    def writeFileRecord(self, slaveaddr, file_number, record_number, record_data):
        record = FileRecord(file_number=1, record_number=record_number, record_data=record_data)
        self.client.write_file_record([record], slave=slaveaddr)

class Panel:
    def __init__(self, modbus):
        self.modbus = modbus
        self.slaveaddr = 1

    target_save_settings = property(lambda self: self.modbus.read_u16(0x010F, self.slaveaddr),
                                   lambda self, val: self.modbus.write_u16(0x010F, val, self.slaveaddr))
    target_reboot = property(lambda self: self.modbus.read_u16(0x0111, self.slaveaddr),
                             lambda self, val: self.modbus.write_u16(0x0111, val, self.slaveaddr))

    def readVersion(self):
        major = self.modbus.read_u16(0x0000, self.slaveaddr)
        minor = self.modbus.read_u16(0x0001, self.slaveaddr)
        patch = self.modbus.read_u16(0x0002, self.slaveaddr)
        return major, minor, patch

    sn = property(lambda self: self.modbus.read_i32(0x0004, self.slaveaddr), None)

    odoPulseCount = property(lambda self: self.modbus.read_u32(0x0300, self.slaveaddr), None)
    pulseCount = property(lambda self: self.modbus.read_u32(0x0302, self.slaveaddr), None)
    pulseCountpm = property(lambda self: self.modbus.read_u32(0x0304, self.slaveaddr), None)
    countTime = property(lambda self: self.modbus.read_u32(0x0306, self.slaveaddr), None)
    rad_uR = property(lambda self: self.modbus.read_u32(0x0308, self.slaveaddr) / 10.0, None)
    rad_uSv = property(lambda self: self.modbus.read_u32(0x030A, self.slaveaddr) / 100.0, None)
    radVal_uRph = property(lambda self: self.modbus.read_u32(0x030C, self.slaveaddr) / 10.0, None)
    radVal_uSvph = property(lambda self: self.modbus.read_u32(0x030E, self.slaveaddr) / 100.0, None)

    def updateFw(self, filePath):
        data = Path(filePath).read_bytes()
        fwLen = len(data)
        pbar = tqdm(total=fwLen, desc="Loading...", unit='B')
        for offset in range(0, fwLen, 128):
            bytesToWrite = 128 if fwLen - offset >= 128 else fwLen - offset
            pbar.update(bytesToWrite)
            record_number = int(offset / 128)
            self.modbus.writeFileRecord(self.slaveaddr, file_number=1, record_number=record_number,
                                        record_data=data[offset: offset + bytesToWrite])
        pbar.close()
        self.target_reboot = 1
        print('wait for boot (10-20s)')
        time.sleep(2)

class Ps3604l:
    def __init__(self, ip):
        self.modbus = Modbus(ip)
        self.panel = Panel(self.modbus)
        pmajor, pminor, ppatch = self.panel.readVersion()
        print(f'Version P{pmajor}.{pminor}.{ppatch} sn {self.panel.sn}')

if __name__ == '__main__':
    ap = argparse.ArgumentParser(description='API Nucci')
    ap.add_argument('-i', '--ipaddr', required=True, help='Device IP address')
    ap.add_argument("-p", "--fwpanel", required=False, help="Firmware panel file")
    ap.add_argument('-w', help='Wait', action='store_true')
    args = ap.parse_args()

    ps = Ps3604l(args.ipaddr)

    if args.fwpanel:
        print('Do you want write firmware to panel? [y/n]')
        resp = input()
        if resp != 'y' and resp != 'Y':
            print('Exit')
            quit()
        ps.panel.updateFw(args.fwpanel)
        time.sleep(1)
        raise SystemExit

    while True:
        try:
            print(f'odoPulseCount: {ps.panel.odoPulseCount}')
            print(f'pulseCount:    {ps.panel.pulseCount}')
            print(f'pulseCountpm:  {ps.panel.pulseCountpm} cmp/m')
            print(f'countTime:     {ps.panel.countTime} s')
            print(f'rad_uR:        {ps.panel.rad_uR} μR')
            print(f'radVal_uRph:   {ps.panel.radVal_uRph} μR/h')
            lineUp = '\x1B[F'
            time.sleep(0.5)
            print(lineUp + lineUp + lineUp + lineUp + lineUp + lineUp, end='')

        except KeyboardInterrupt:
            print('\n')
            break
