import wave, os, pyaudio, time
import hid # https://pypi.python.org/pypi/hidapi aka https://github.com/trezor/cython-hidapi

_pyaudio_instance = pyaudio.PyAudio()
_wav = None
RESPEAKER_VENDOR_ID = 0x2886
RESPEAKER_PRODUCT_ID = 0x07
RECORD_SR = 16000

# Set up the HID driver
_dev = hid.device()
_dev.open(RESPEAKER_VENDOR_ID, RESPEAKER_PRODUCT_ID)

# Write data to a register, return how many bytes were written
def write_register(register, data):
    send_data = [0, register, 0, len(data), 0 ] + data
    return _dev.write(send_data)

# Read length data from a register, return the data
def read_register(register, length):
    # To read a register you send reg & 0x80, and then read it back
    # If you have blocking off the read will return none if it's too soon after
    send_data = [0, register, 0x80, length, 0, 0, 0]
    what = _dev.write(send_data)
    ret = _dev.read(len(send_data) + length)
    return ret[4:4+length] # Data comes in at the 4th byte

# See if there's any auto report (VAD and voice angle), if not, return immediately with None
def read_auto_report():
    # Temporarily turn off blocking, the auto report only comes in on VAD changes
    _dev.set_nonblocking(1)
    ret = _dev.read(9)
    _dev.set_nonblocking(0)
    if(len(ret)):
        # Make sure it's the auto report register (0xFF)
        if(ret[0] == 0xFF):
            # Angle is two bytes
            angle = ret[6]*256 + ret[5]
            # VAD is 2 for speaking, 0 for not, 1 for ???
            vad = ret[4]
            return (angle, vad)
    return (None, None)

# Get the audio device ID for pyaudio
def get_respeaker_audio_device():
    device_index = None
    for i in range(_pyaudio_instance.get_device_count()):
        dev = _pyaudio_instance.get_device_info_by_index(i)
        name = dev['name'].encode('utf-8')
        if name.lower().find('respeaker') >= 0 and dev['maxInputChannels'] > 0:
            return i
    return -1

# Audio reading callback, just write to a wav file
def callback(in_data, frame_count, time_info, status):
    global _wav
    _wav.writeframes(in_data)
    return None, pyaudio.paContinue

# Record audio for length seconds while writing any VAD and angle changes to the screen
def record(length_seconds=10, filename="output.wav"):
    global _wav
    if(os.path.exists(filename)):
        os.remove(filename)

    _wav = wave.open(filename, 'wb')
    _wav.setframerate(RECORD_SR) 
    _wav.setsampwidth(2) # shorts
    _wav.setnchannels(1) # 1 channel 

    device_index = get_respeaker_audio_device()
    if(device_index < 0):
        print "cannot find audio device"
        sys.exit()

    stream = _pyaudio_instance.open(
        input=True, 
        start=False,
        format=pyaudio.paInt16,
        channels=1,rate=RECORD_SR, 
        frames_per_buffer=512, 
        stream_callback=callback,  
        input_device_index=device_index 
    )
    stream.start_stream()

    start = time.time()
    while(time.time() - start < length_seconds):
        (angle, vad) = read_auto_report()
        if(angle is not None):
            print "time %2.5f angle: %d vad: %d" % (time.time() - start, angle, vad)
        time.sleep(0.001)
    
    stream.stop_stream()
    _wav.close()



def main():
    # Test reading a register, like the mic gain (set to 30dB by factory)
    # See registers.csv for the full list of what is get/settable
    mic_gain = read_register(0x10, 1)[0]
    print "Mic gain is set to %d" % (mic_gain)
    # And test writing a register, the LED -- I turn them off, saves ~120 mA
    write_register(0, [0, 0, 0, 0]) # mode, b, g, r  -- mode 0 is "all off"
    # Now record audio to a wav file for 10 seconds and print VAD and angle to screen
    record(length_seconds = 10)

if __name__ == '__main__':
    main()
