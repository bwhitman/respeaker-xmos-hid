# respeaker-xmos-hid

Python & C hidapi examples for the "Respeaker" microphone array to show angle &amp; voice activity (VAD) information.

For both, you need to install [hidapi.](https://github.com/signal11/hidapi)

For Python, you need this [hidapi wrapper](https://pypi.python.org/pypi/hidapi). (Note: there are two python modules called hid, this is the one you get by pip install hidapi. The other probably works too but has different calling conventions.)

The Python example records 10 seconds to a wave file called output.wav and prints out VAD and voice angle information in real time to the screen. Just plug the mic array directly over USB to your computer and run the script.

```
carry:respeaker-xmos-hid bwhitman$ python listen_and_get_position.py 
Mic gain is set to 30
time 0.88428 angle: 30 vad: 2
time 3.54835 angle: 30 vad: 0
time 3.97157 angle: 330 vad: 2
time 5.57193 angle: 330 vad: 0
time 6.38029 angle: 150 vad: 2
time 7.85256 angle: 150 vad: 0
time 8.37984 angle: 150 vad: 2
time 8.97196 angle: 150 vad: 0
time 9.16385 angle: 150 vad: 2
time 9.81978 angle: 150 vad: 0
```




