# Written by Kai Seshimo (Semester 1, 2022) 

from redmail import outlook #pip install redmail
from download_azure_records import check_device_status

OUTLOOK_EMAIL_ADDRESS  = "XXX"
OUTLOOK_EMAIL_PASSWORD = "XXX"

NUM_DEVICES            = 6   #number of total IoT devices. (Default: 6)

def send_email():
    stopped_device_num = check_device_status(NUM_DEVICES)  
    outlook.username = OUTLOOK_EMAIL_ADDRESS
    outlook.password = OUTLOOK_EMAIL_PASSWORD   
    
    if (len(stopped_device_num)):
        text = "Hi, Device " + str(stopped_device_num) + " stopped working."
    else:
        text = "Hi, all devices are working."

    outlook.send(
        receivers = [OUTLOOK_EMAIL_ADDRESS],
        subject = "PARbar NBIoT",
        text = text
    )  


if __name__ == "__main__":
    send_email() 