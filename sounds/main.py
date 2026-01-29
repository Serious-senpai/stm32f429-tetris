import argparse
import time
from pathlib import Path

import pygame
import serial


class Namespace(argparse.Namespace):
    port: str
    rate: int


WAV = Path(__file__).parent / "wav"


parser = argparse.ArgumentParser(
    description="Play sound effects based on serial messages from a STM32",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter,
)
parser.add_argument("port", type=str, help="Serial port to listen on")
parser.add_argument(
    "-r",
    "--rate",
    type=int,
    default=115200,
    help="Baud rate for the serial connection",
)

namespace = parser.parse_args(namespace=Namespace())

SOUNDS = {
    "CONTROL": WAV / "SFX_PieceMoveLR.wav",
    "GAMEOVER": WAV / "SFX_GameOver.wav",
    "SCORE": WAV / "SFX_SpecialLineClearTriple.wav",
}

# Open serial port
ser = serial.Serial(namespace.port, namespace.rate, timeout=1)
print(f"Listening on {namespace.port} at {namespace.rate} baud...")

pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=512)
pygame.mixer.set_num_channels(8)
sounds = {k: pygame.mixer.Sound(str(v)) for k, v in SOUNDS.items()}

try:
    while True:
        if ser.in_waiting > 0:
            payload = ser.read_until(b"\0").decode(errors="ignore").strip("\0")
            if payload:
                print(f"Received: {payload!r}")

                try:
                    sounds[payload].play()
                except KeyError:
                    print("Unrecognized payload")

        # Tiny sleep prevents 100% CPU usage
        time.sleep(0.01)

except KeyboardInterrupt:
    print("\nReceived Ctrl-C signal")

finally:
    ser.close()
