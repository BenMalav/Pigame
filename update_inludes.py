#!/usr/bin/python3

import time
import paramiko 
import getpass
import os
import subprocess

hostname = '192.168.1.74'
username = 'pi'
port = 22
passwd = getpass.getpass('Enter remote PW:')

dir_name = 'headers'
client = paramiko.SSHClient()

include_dirs = ['usr']
lib_dirs = ['']

def ssh_exec_blocking(cmd):
    stdin, stdout, stderr = client.exec_command(cmd)
    exit_status = stdout.channel.recv_exit_status()

    if (exit_status > 0):
        print ('command failed on remote...!')
    else:
        print('command ok!')

def create_dir():
    chan = client.get_transport().open_session()
    
    print('dir name.. {}'.format(dir_name))
    cmd = 'mkdir ' + dir_name

    chan.exec_command(cmd)

    if (chan.recv_exit_status() > 0):
        print("Directory already exits..")


def find_n_copy():
    global dir_name
    global include_dirs
    global client 

    for i in include_dirs:
        cmd = 'find /{}/ -name "*.h" -print0 | xargs -0 -I_ cp --parents _ {}/'.format(i, dir_name)
        ssh_exec_blocking(cmd)
'''
    for i in lib_dirs:
        cmd = 'find /{}/ -name "*.so" -print0 | xargs -0 -I_ cp --parents _ {}/'.format(i, dir_name)
        ssh_exec_blocking(cmd)
'''


def compress_dir():
    global dir_name
    cmd = 'tar -czvf {}.tar.gz {}'.format(dir_name, dir_name)
    ssh_exec_blocking(cmd)


def start_paramiko():
    global client 
    client.load_system_host_keys()
    client.set_missing_host_key_policy(paramiko.WarningPolicy)
    client.connect(hostname, port=port, username=username, password=passwd)


def copy_to_host():
    global client
    global dir_name

    sftp = client.open_sftp()

    stdin, stdout, stderr = client.exec_command('pwd')
    cwd = stdout.read().decode("utf-8").rstrip()
    remote_file = ''
    remote_file = cwd + '/' + dir_name + '.tar.gz'

    local_file = os.getcwd() + '/headers.tar.gz'
    
    sftp.get(remote_file, local_file)
    
    ssh_exec_blocking('rm -rf {}'.format(dir_name))
    ssh_exec_blocking('rm -rf {}.tar.gz'.format(dir_name))


def extract_on_host():
    global dir_name

    p = subprocess.Popen(['tar', '-xzvf', 'headers.tar.gz'])
    p.wait()
    p = subprocess.Popen(['rm', '-f', 'headers.tar.gz'])
    p.wait()
    subprocess.Popen(['mv', '-fv', dir_name, 'headers'])


def main():
    global dir_name
    dir_name += time.strftime("%Y%m%d-%H%M%S")

    start_paramiko()
    create_dir()
    find_n_copy()
    compress_dir()
    copy_to_host()
    extract_on_host()

if __name__ == "__main__":
    main()
