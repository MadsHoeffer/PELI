import socket
import numpy as np
import soundfile as sf
import paho.mqtt.client as mqtt
from openai import OpenAI

# This code requires that an OpenAPI key is export in your bash aliases.


MIC_OFFSET = 1551
client = OpenAI()
msg = ""
#Sampling frequenc
fs = 10000
BUFFER_SIZE = 25000    
audio = list()
audio_file_written = False

def on_message(client, userdata, message):
    global msg
    msg = str(message.payload.decode('utf-8'))
    print("MQTT: ",msg)

def send_GPT_request(audio_file_path):
    # Read audio file
    audio_file= open(audio_file_path, "rb")
    # Speech-to-Text
    transcript = client.audio.transcriptions.create(
    model="whisper-1", 
    file=audio_file
    )
    #ChatGPT query
    completion = client.chat.completions.create(
    model="gpt-3.5-turbo",
    messages = [{"role": "user",
                 "content": str(transcript.text)}]
    )
    return transcript.text,(completion.choices[0].message.content)

def setup_MQTT():
    client = mqtt.Client("Mads2")
    client.username_pw_set("Team4", "Team4")
    broker_address="192.168.8.167"
    # broker_address="team4.wlan"

    client.on_message = on_message
    client.connect(broker_address)
    client.subscribe("audio/alert",0) 
    client.loop_start()
    return client
def main():
    # HOST = '192.168.10.196' # RPI 
    HOST = "192.168.8.167" # PC
    # Listen on Port 
    PORT = 44444

    # Create a UDP socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Bind the socket to the host and port
    s.bind((HOST, PORT))
    MQTT_client = setup_MQTT()
    while True:
        if(msg != "Start"):
            d = s.recv(BUFFER_SIZE)
        if (msg == "Start"):
            audio_file_written = False
            data = s.recvfrom(BUFFER_SIZE)
            if data:
                mv = list(memoryview(data[0]))
                for i in range(0,len(mv),2):
                    # Convert two 8 bit values to the original 16 bit audio sample. 
                    val = ((mv[i]<<8|mv[i+1]) - MIC_OFFSET + 2048)
                    audio.append(val)
        if(msg == "Stop" and audio_file_written == False):
            audio_file_written = True
            audio_play = np.array(audio,dtype=np.int16)
            audio_play = (audio_play/4095-0.5)*2
            sf.write('speech.wav',audio_play,fs)
            question,response = send_GPT_request("speech.wav")
            MQTT_client.publish("gpt/question",question)
            MQTT_client.publish("gpt/response",response)
            audio.clear()

        


    # Close connection
    s.close()

if __name__ == "__main__":
    main()