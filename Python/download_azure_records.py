# Written by Kai Seshimo (Semester 1, 2022) 

import os
from azure.storage.blob import BlobServiceClient, BlobClient, ContainerClient, __version__ #pip install azure-storage-blob
from json import JSONDecoder
import base64
import csv
from record_translator import get_dev_list, get_PAR_lists, check_file_closed

#PARbarIoTHub2
STORAGE_ACCOUNT_URL = "XXX"
STORAGE_ACCOUNT_KEY = "XXX"
CONNECTION_STRING   = "XXX"

CONTAINER_NAME      = "parbar"

FILE_NAME_BLOB_DEV1  = "blob_dev1.csv"
FILE_NAME_BLOB_DEV2  = "blob_dev2.csv"
FILE_NAME_BLOB_DEV3  = "blob_dev3.csv"
FILE_NAME_BLOB_DEV4  = "blob_dev4.csv"
FILE_NAME_BLOB_DEV5  = "blob_dev5.csv"
FILE_NAME_BLOB_DEV6  = "blob_dev6.csv"
FILE_NAME_BLOB_DEV   = "blob_dev.csv"
FILE_NAMES_BLOB = [FILE_NAME_BLOB_DEV1, FILE_NAME_BLOB_DEV2, FILE_NAME_BLOB_DEV3, FILE_NAME_BLOB_DEV4, FILE_NAME_BLOB_DEV5, \
                    FILE_NAME_BLOB_DEV6, FILE_NAME_BLOB_DEV]

def extract_json_objects(text, decoder=JSONDecoder()):
    """Find JSON objects in text, and yield the decoded JSON data

    Does not attempt to look for JSON arrays, text, or other JSON types outside
    of a parent JSON object.

    """
    pos = 0
    while True:
        match = text.find('{', pos)
        if match == -1:
            break
        try:
            result, index = decoder.raw_decode(text[match:])
            yield result
            pos = match + index
        except ValueError:
            pos = match + 1

def add_headers(file_name_PAR, num_devices, num_ports):
    for file_name in FILE_NAMES_BLOB:
        if (os.path.isfile(file_name)):
            with open(file_name, "r") as file:
                if (len(file.readlines())):
                    continue
        with open(file_name, 'w', encoding = 'UTF8', newline = '') as file:
            writer = csv.writer(file)
            header = ['DATE', 'TIME', 'ID']
            for y in range(0, num_ports):
                header.append('A' + str(y) + ' (mV)')
            header.append('BATTERY') 
            header.append('SIGNAL')
            writer.writerow(header)
    
    if (os.path.isfile(file_name_PAR)):
        with open(file_name_PAR, "r") as file:
            if (len(file.readlines())):
                return 
    with open(file_name_PAR, 'w', encoding = 'UTF8', newline = '') as file:
        writer = csv.writer(file)
        header = ['DATE', 'TIME']
        for x in range(0, num_devices):
            for y in range(0, num_ports):
                header.append('Device' + str(x + 1) + ':PAR' + str(y))
        writer.writerow(header)

def download_azure_records(file_name_PAR, num_devices, num_ports, publish_period_minutes):          
    dirname, filename = os.path.split(os.path.abspath(__file__))
    os.chdir(dirname)
    if not os.path.isdir('data'):
        os.mkdir('data')
    os.chdir('data')
    # cwd = os.getcwd()
    # print("Current working directory: {0}".format(cwd))
    file_names = FILE_NAMES_BLOB.copy()
    file_names.append(file_name_PAR)
    check_file_closed(file_names)
    add_headers(file_name_PAR, num_devices, num_ports) 

    PAR_lists = []
    try:
        print("\n")
        print("Start Downloading records from Azure...")
        # Create the BlobServiceClient object which will be used to create a container client
        blob_service_client = BlobServiceClient.from_connection_string(CONNECTION_STRING)
        
        container = ContainerClient.from_connection_string(conn_str = CONNECTION_STRING, container_name = CONTAINER_NAME)
        blob_list = container.list_blobs()
        combined_dev_lists = []

        for blob in blob_list:
            blob_client = blob_service_client.get_blob_client(container = CONTAINER_NAME, blob = blob.name)
            text = blob_client.download_blob().readall().decode("utf-8")
            for result in extract_json_objects(text):
                # the result is a Python dictionary:
                line = base64.b64decode(result["Body"]).decode("utf-8")
                print("-> downloading " + blob.name + ", context: " + line)
                dev_list = get_dev_list(line.split(","))
                device_id = dev_list[2]
                if (1 <= device_id and device_id <= num_devices):
                    combined_dev_lists.append(dev_list)

                if (device_id == 1):
                    file_name =  FILE_NAME_BLOB_DEV1
                elif  (device_id == 2):
                    file_name =  FILE_NAME_BLOB_DEV2
                elif  (device_id == 3):
                    file_name =  FILE_NAME_BLOB_DEV3
                elif  (device_id == 4):
                    file_name =  FILE_NAME_BLOB_DEV4
                elif  (device_id == 5):
                    file_name =  FILE_NAME_BLOB_DEV5
                elif  (device_id == 6):
                    file_name =  FILE_NAME_BLOB_DEV6
                else: 
                    file_name =  FILE_NAME_BLOB_DEV
            
                try:
                    with open(file_name, 'a', encoding = 'UTF8', newline = '') as file:
                        writer = csv.writer(file)
                        writer.writerow(dev_list)

                except Exception as error: 
                    print("Coultn't open " + file_name)

        PAR_lists = get_PAR_lists(combined_dev_lists, num_devices, num_ports, publish_period_minutes)
        try: 
            with open(file_name_PAR, 'a', encoding = 'UTF8', newline = '') as file:
                writer = csv.writer(file)
                for PAR_list in PAR_lists:
                    writer.writerow(PAR_list)
            
            blob_list = container.list_blobs()

            # below to delete the data on Azure
            # for blob in blob_list:
            #     print("-> deleting " + blob.name)
            #     blob_client = blob_service_client.get_blob_client(container=CONTAINER_NAME, blob=blob.name)
            #     container.delete_blob(blob.name)
                
        except Exception as error: 
            print("Couldn't open " + file_name_PAR)
            print(error)
            return
            
        print("Done!" + " " + file_name_PAR + " can be found under 'data' folder.") 
        print("\n")

    except Exception as ex:
        print('Exception:')
        print(ex)
    return PAR_lists

def check_device_status(num_devices):          
    stopped_device_list = []
    try:
        # Create the BlobServiceClient object which will be used to create a container client
        blob_service_client = BlobServiceClient.from_connection_string(CONNECTION_STRING)
        
        container = ContainerClient.from_connection_string(conn_str=CONNECTION_STRING, container_name=CONTAINER_NAME)
        blob_list = container.list_blobs()
        count_array = [0 for i in range(num_devices)] 
        for blob in blob_list:
            blob_client = blob_service_client.get_blob_client(container=CONTAINER_NAME, blob=blob.name)
            text = blob_client.download_blob().readall().decode("utf-8")
            for result in extract_json_objects(text):
                # the result is a Python dictionary:
                line = base64.b64decode(result["Body"]).decode("utf-8")
                # print("-> downloading " + blob.name + ", context: " + line)
                dev_list = get_dev_list(line.split(","))
                device_id = dev_list[2]
                
                if (1 <= device_id and device_id <= 6):
                    count_array[device_id - 1] += 1 

        for idx in range(0, len(count_array)):
            if (count_array[idx] < max(count_array) / 2):
                stopped_device_list.append(idx + 1)
        
        if (len(count_array) == 0):
            for idx in range(0, num_devices):
                stopped_device_list.append(idx + 1)
            
    except Exception as ex:
        print('Exception:')
        print(ex)

    return stopped_device_list


if __name__ == "__main__":
    FILE_NAME_PAR = "Test_PARdata.csv"
    NUM_DEVICES = 6
    NUM_PORTS = 6
    PUBLISH_PERIOD_MINUTE = 3 
    download_azure_records(FILE_NAME_PAR, NUM_DEVICES, NUM_PORTS, PUBLISH_PERIOD_MINUTE)
    