#  MATMUL-16 CORES.MK
#
#  Copyright (C) 2012 Adapteva, Inc.
#  Contributed by Yaniv Sapir <yaniv@adapteva.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program, see the file COPYING.  If not, see
#  <http://www.gnu.org/licenses/>.


ALL_ELF += ../$(TARGET).core.32_36/$(CONF)/$(TARGET).core.32_36.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.32_36.srec 
./Cores/$(TARGET).core.32_36.srec : ../$(TARGET).core.32_36/$(CONF)/$(TARGET).core.32_36.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x824\>
	@sed -i 's/^S0\(..\)0000/S0\10824/' "$@"
	

ALL_ELF += ../$(TARGET).core.32_37/$(CONF)/$(TARGET).core.32_37.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.32_37.srec 
./Cores/$(TARGET).core.32_37.srec : ../$(TARGET).core.32_37/$(CONF)/$(TARGET).core.32_37.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x825\>
	@sed -i 's/^S0\(..\)0000/S0\10825/' "$@"


ALL_ELF += ../$(TARGET).core.32_38/$(CONF)/$(TARGET).core.32_38.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.32_38.srec 
./Cores/$(TARGET).core.32_38.srec : ../$(TARGET).core.32_38/$(CONF)/$(TARGET).core.32_38.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x826\>
	@sed -i 's/^S0\(..\)0000/S0\10826/' "$@"


ALL_ELF += ../$(TARGET).core.32_39/$(CONF)/$(TARGET).core.32_39.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.32_39.srec 
./Cores/$(TARGET).core.32_39.srec : ../$(TARGET).core.32_39/$(CONF)/$(TARGET).core.32_39.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x827\>
	@sed -i 's/^S0\(..\)0000/S0\10827/' "$@"


ALL_ELF += ../$(TARGET).core.33_36/$(CONF)/$(TARGET).core.33_36.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.33_36.srec 
./Cores/$(TARGET).core.33_36.srec : ../$(TARGET).core.33_36/$(CONF)/$(TARGET).core.33_36.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x864\>
	@sed -i 's/^S0\(..\)0000/S0\10864/' "$@"


ALL_ELF += ../$(TARGET).core.33_37/$(CONF)/$(TARGET).core.33_37.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.33_37.srec 
./Cores/$(TARGET).core.33_37.srec : ../$(TARGET).core.33_37/$(CONF)/$(TARGET).core.33_37.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x865\>
	@sed -i 's/^S0\(..\)0000/S0\10865/' "$@"


ALL_ELF += ../$(TARGET).core.33_38/$(CONF)/$(TARGET).core.33_38.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.33_38.srec 
./Cores/$(TARGET).core.33_38.srec : ../$(TARGET).core.33_38/$(CONF)/$(TARGET).core.33_38.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x866\>
	@sed -i 's/^S0\(..\)0000/S0\10866/' "$@"


ALL_ELF += ../$(TARGET).core.33_39/$(CONF)/$(TARGET).core.33_39.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.33_39.srec 
./Cores/$(TARGET).core.33_39.srec : ../$(TARGET).core.33_39/$(CONF)/$(TARGET).core.33_39.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x867\>
	@sed -i 's/^S0\(..\)0000/S0\10867/' "$@"


ALL_ELF += ../$(TARGET).core.34_36/$(CONF)/$(TARGET).core.34_36.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.34_36.srec 
./Cores/$(TARGET).core.34_36.srec : ../$(TARGET).core.34_36/$(CONF)/$(TARGET).core.34_36.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8a4\>
	@sed -i 's/^S0\(..\)0000/S0\108a4/' "$@"


ALL_ELF += ../$(TARGET).core.34_37/$(CONF)/$(TARGET).core.34_37.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.34_37.srec 
./Cores/$(TARGET).core.34_37.srec : ../$(TARGET).core.34_37/$(CONF)/$(TARGET).core.34_37.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8a5\>
	@sed -i 's/^S0\(..\)0000/S0\108a5/' "$@"


ALL_ELF += ../$(TARGET).core.34_38/$(CONF)/$(TARGET).core.34_38.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.34_38.srec 
./Cores/$(TARGET).core.34_38.srec : ../$(TARGET).core.34_38/$(CONF)/$(TARGET).core.34_38.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8a6\>
	@sed -i 's/^S0\(..\)0000/S0\108a6/' "$@"


ALL_ELF += ../$(TARGET).core.34_39/$(CONF)/$(TARGET).core.34_39.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.34_39.srec 
./Cores/$(TARGET).core.34_39.srec : ../$(TARGET).core.34_39/$(CONF)/$(TARGET).core.34_39.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8a7\>
	@sed -i 's/^S0\(..\)0000/S0\108a7/' "$@"


ALL_ELF += ../$(TARGET).core.35_36/$(CONF)/$(TARGET).core.35_36.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.35_36.srec 
./Cores/$(TARGET).core.35_36.srec : ../$(TARGET).core.35_36/$(CONF)/$(TARGET).core.35_36.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8e4\>
	@sed -i 's/^S0\(..\)0000/S0\108e4/' "$@"


ALL_ELF += ../$(TARGET).core.35_37/$(CONF)/$(TARGET).core.35_37.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.35_37.srec 
./Cores/$(TARGET).core.35_37.srec : ../$(TARGET).core.35_37/$(CONF)/$(TARGET).core.35_37.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8e5\>
	@sed -i 's/^S0\(..\)0000/S0\108e5/' "$@"


ALL_ELF += ../$(TARGET).core.35_38/$(CONF)/$(TARGET).core.35_38.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.35_38.srec 
./Cores/$(TARGET).core.35_38.srec : ../$(TARGET).core.35_38/$(CONF)/$(TARGET).core.35_38.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8e6\>
	@sed -i 's/^S0\(..\)0000/S0\108e6/' "$@"


ALL_ELF += ../$(TARGET).core.35_39/$(CONF)/$(TARGET).core.35_39.elf 
ALL_SREC +=   ./Cores/$(TARGET).core.35_39.srec 
./Cores/$(TARGET).core.35_39.srec : ../$(TARGET).core.35_39/$(CONF)/$(TARGET).core.35_39.elf
	@mkdir -p ./Cores/
	@e-objcopy --srec-forceS3 --output-target srec "$<" "$@"
	@echo Creating srec file for CoreID\<0x8e7\>
	@sed -i 's/^S0\(..\)0000/S0\108e7/' "$@"


