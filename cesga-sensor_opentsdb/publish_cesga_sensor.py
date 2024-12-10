#!/usr/libexec/platform-python
# encoding: utf-8
"""Publish CESGA sensor values information

This script takes care of collecting clima conditions of the CESGA sensor over the internet
There are several environmental variables that can be used to control its
execution:

- DEBUG: Produce more verbose logging
- DEMO: Do not publish data, just retrieve them

Example:
    DEBUG=1 DEMO=1 python3 publish_gpu_usage.py
"""
import logging
import time
import json
from urllib.request import urlopen, Request
from subprocess import Popen, PIPE
import gzip
import glob
import re
import socket
import os

ENDPOINT = '******'     # Add DB endpoint


def configure_logging():
    """Configure logging"""
    if 'DEBUG' in os.environ:
        logging.basicConfig(format='%(asctime)-15s %(levelname)s: %(message)s',
                            level=logging.DEBUG)
    else:
        logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)


def publish(gzpayload):
    """Publish data to TSDB"""
    try:
        req = Request(ENDPOINT)
        req.add_header('Content-Type', 'application/json')
        req.add_header('Content-Encoding', 'gzip')

        response = urlopen(req, gzpayload)

        if response.code != 204:
            logging.error('Error publishing data!!!')
            logging.info('Response code received:')
            logging.info(response.code)
        else:
            logging.debug('Data published correctly')
    except Exception as e:
        logging.error('Unknown error while publishing data. Will retry on next iteration')
        logging.error(str(e))


def compress(payload):
    """Compress the payload"""
    # The default encoding for JSON is UTF-8
    return gzip.compress(json.dumps(payload).encode('utf-8'))


def compress_and_publish(payload):
    """Compress and publish the payload to TSDB"""
    gzpayload = compress(payload)
    logging.debug('Compressed payload size: {} bytes'.format(len(gzpayload)))
    if 'DEMO' in os.environ:
        logging.info('DEMO MODE: Not Publishing sensors')
    else:
        publish(gzpayload)


def read_multi(filename):
    """Reads a file with multiple data separated by space and returns a dict"""
    result = {}
    with open(filename) as data:
        for line in data:
            key, value = line.split()
            result[key] = value
    return result


def read_single(filename):
    """Reads a file with a single value"""
    with open(filename) as data:
        for line in data:
            return line.strip()


def read_command_multi(command):
    """Reads the output of a command and returns the splitted result"""
    proc = Popen(command.split(), stdout=PIPE)
    out, errors = proc.communicate()
    lines = out.decode('utf-8').split('\n')[:-1]
    return [l.split() for l in lines]

def read_http_sensor(URL):
    """Reads the output of sensor that publish json and returns a dict"""
    remote_sensor = json.loads(urlopen(URL).read().decode('utf-8')[:-4])
    #lines = remote_sensor.split()
    return remote_sensor

def retrieve_metrics():
    payload = []
    now = int(time.time())
    hostname = socket.gethostname()

    URL_sensor = 'http://10.212.5.30'
    lines = read_http_sensor(URL_sensor)
    for k in lines:
        payload.append(
                {'metric': f'CESGA_sensor.{k}',
                 'value': float(lines[k]),
                 'timestamp': now,
                 'tags': {'host': hostname}})
    return payload

if __name__ == '__main__':
    configure_logging()
    payload = retrieve_metrics()
    logging.debug(payload)
    compress_and_publish(payload)
