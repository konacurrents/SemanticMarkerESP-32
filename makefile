	
#upload (after compiling) a script for the ESP32
#NOTE: FQBN stands for 'Fully Qualified Board Name'. When compiling or uploading code, this error will occur if no board is selected, or if the board 

compile:
	/opt/homebrew/bin/arduino-cli compile --build-property compiler.cpp.extra_flags=-DESP_32_FEEDER --fqbn esp32:esp32:esp32 | tee output

compileBoard:
	/opt/homebrew/bin/arduino-cli compile --build-property compiler.cpp.extra_flags=-DESP_32_FEEDER_WITH_BOARD --fqbn esp32:esp32:esp32 | tee output


compileM5:
	/opt/homebrew/bin/arduino-cli compile --build-property compiler.cpp.extra_flags=-DESP_M5_SMART_CLICKER_CONFIGURATION --fqbn esp32:esp32:m5stick-c | tee output

upload:
	/opt/homebrew/bin/arduino-cli upload --port /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 | tee  output
	
clearFlash:
	/opt/homebrew/bin/esptool.py --chip esp32 --port /dev/cu.usbserial-0001 erase_flash
	

# requires doxygen and graphviz
doxygen:
	rm -rf html latex
	doxygen doxconfig
	tar cfz h.tar.gz html

uploadFlashOLD: .
	scp ESP_IOT.ino.esp32.bin wave@konacurrents.com:.
#   scp ESP_IOT.ino.m5stick_c_plus.bin wave@konacurrents.com:.
#then move to WebServer/Docments root (which is root of konacurrents.com

# MAIN are all at OTA/TEST/MAIN

# Installed 'main' (but not the main folder, as the ESP device doesn't use it..)
#  #OTA {k:ESP_32) .. will find the uploadFlashBoard location (OTA/OTA_Boaed)
uploadFlash_orig: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.esp32.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA

uploadFlashBoard: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.esp32.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/OTA_Board

uploadFlashM5: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stick_c_plus.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA

uploadFlashM5QRReader: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stick_c_plus.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/M5QRReader
	
# TEST/MAIN is being depreciated
uploadFlashBoard_MAIN: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.esp32.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/MAIN

uploadFlashM5_MAIN: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stick_c_plus.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/MAIN

## QA Testing

uploadFlashBoard_QA: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.esp32.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/QA_OTA_Board

uploadFlashM5_QA: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stick_c_plus.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/QA_M5

## Testing of DEV

# feeder original (pumpkin uno, scooby doo)
uploadFlashOrigTEST: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.esp32.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/ORIG

# MAIN feeder
uploadFlashBoardTEST: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.esp32.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/OTA_Board

uploadFlashM5TEST: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stick_c_plus.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/M5

uploadFlashM5CameraTEST: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stack_timer_cam.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/M5

uploadFlashM5QRReaderTEST: .
	scp -i ~/.ssh/idogwatch_001.pem ESP_IOT.ino.m5stick_c_plus.bin ec2-user@knowledgeshark.me:html/KnowledgeShark.org/OTA/TEST/M5QRReader
	
	
releaseFull: compileM5  compileBoard
	tar cvfz releases.tar.gz ESP_IOT.ino.esp32.bin ESP_IOT.ino.m5stick_c_plus.bin

release: 
	tar cvfz releases.tar.gz ESP_IOT.ino.esp32.bin ESP_IOT.ino.m5stick_c_plus.bin ESP_IOT.ino.m5stack_timer_cam.bin
