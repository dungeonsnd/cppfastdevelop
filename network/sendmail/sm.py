#!/usr/bin/env python
# -*- coding: utf-8 -*-

# python发送邮件脚本 v0.4
# 程序功能: 一段正文，并可附带若干文件作为附件。
# 作者:jeffery  ( dungeonsnd at gmail dot com, http://blog.csdn.net/dungeonsnd )
# 2011-07-10 GZ
# 代码可以从作者的CSDN资源里下载到(http://download.csdn.net/user/dungeonsnd),演示可以从作者blog中查看.

# v0.1 第一版本
# v0.2 修改了密码输入
# v0.3 1. 把-d选项去掉，让用户直接输入sm directory|file，省了"-d "这几个字母，如果没有输入这个参数，那么会去配置文件中寻找 diretory项。2. 增加输出 发送起止时间和总共用时。
# v1.4 20121220 优化代码结构，方便使用。支持添加邮件正文(使用参数-t传入，或-T提供文件名程序读入内容作为邮件正文)，支持发一送多个文件或文件夹，并且其它模块可以直接调用本模块的SendDir函数来发送文件或文件夹。

# Using example:
# python sm.py -f doc.tar.gz -s dungeonsnd@gmail.com -r dungeonsnd@gmail.com -o smtp.gmail.com -u dungeonsnd@gmail.com -S "Here is subject" -t "Here is text." -T ""

import os,sys,os.path
import time
import datetime
import getpass # password
import optparse # option parser
import smtplib # send email net library

import mimetypes # For guessing MIME type based on file name extension
from email import encoders
#from email.message import Message
from email.mime.audio import MIMEAudio
from email.mime.base import MIMEBase
from email.mime.image import MIMEImage
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


usage_info =""" sm -f filename1 [-f filename2] [...] -s sender -r recipient1 [-r recipient2] [...] -o host -u username [-p password] [-t text|-T textfile] -S subject """ 

help_info =""" Send the a directory or a file as a MIME message.
Only Regular File are sent, and we don't recurse directories. Be careful,folers won't be sent! """

sample_info =""" C:\Python27\python.exe sm.py -f tst.rar -f tst.jpg -s sample@126.com -r sample@126.com -o smtp.126.com -u sample@126.com -S subject -t "" -T abc.txt """

def SendEmail(sender, recipients, 
        host,username,password,
        composed):
    s = smtplib.SMTP(host)
    s.login(username,password)
    s.sendmail(sender, recipients, composed)
    s.quit()

def Files2MimeFormat(sender,recipients,
         host,username,password,
         subject,text,textfile,allFiles):
    # Create the enclosing (outer) message
    outer = MIMEMultipart()
    outer['Subject'] = subject
    outer['To'] = ', '.join(recipients)
    outer['From'] = sender
    outer.preamble = 'You will not see this in a MIME-aware mail reader.\n'
    
    if not text and textfile:
        fp = open(textfile)
        text =fp.read()
        fp.close()
    outer["Accept-Language"]="zh-CN"
    outer["Accept-Charset"]="ISO-8859-1,utf-8"
    text=MIMEText(text)
    text.set_charset("utf-8")
    outer.attach(text)

    for file in allFiles:
        # Guess the content type based on the file's extension.  Encoding
        # will be ignored, although we should check for simple things like gzip'd or compressed files.
        #path =os.path.join(os.getcwd(),file)
        path =os.path.abspath(file)
        ctype, encoding = mimetypes.guess_type(path)
        if ctype is None or encoding is not None:
            # No guess could be made, or the file is encoded (compressed), so se a generic bag-of-bits type.
            ctype = 'application/octet-stream'
        maintype, subtype = ctype.split('/', 1)
        if ctype is None or encoding is not None:
            # No guess could be made, or the file is encoded (compressed), so
            # use a generic bag-of-bits type.
            ctype = 'application/octet-stream'
        maintype, subtype = ctype.split('/', 1)
        if maintype == 'text':
            fp = open(path)
            # Note: we should handle calculating the charset
            msg = MIMEText(fp.read(), _subtype=subtype)
            fp.close()
        elif maintype == 'image':
            fp = open(path, 'rb')
            msg = MIMEImage(fp.read(), _subtype=subtype)
            fp.close()
        elif maintype == 'audio':
            fp = open(path, 'rb')
            msg = MIMEAudio(fp.read(), _subtype=subtype)
            fp.close()
        else:
            fp = open(path, 'rb')
            msg = MIMEBase(maintype, subtype)
            msg.set_payload(fp.read())
            fp.close()
            # Encode the payload using Base64
            encoders.encode_base64(msg)
        # Set the filename parameter
        msg.add_header('Content-Disposition', 'attachment', filename=file)
        outer.attach(msg)
    composed = outer.as_string()
    #print '$$$$',composed,'$$$$'
    return composed;

def PrintStartTime():
    print  '[',time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()),']','Program is trying to send this mail,please wait......' 
    starttime= datetime.datetime.now()
    return starttime
    
def PrintStopTime():
    print '[',time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()),']','Sending finished!'
    stoptime= datetime.datetime.now()
    return stoptime
    
def PrintTotalTime(starttime,stoptime):
    d =stoptime-starttime
    print 'Using time:',str(d)[0:-7]

def ListDirFiles(dir):
    lst =[]
    for root, dirs, files in os.walk(dir):
        for name in files:
            lst.append(os.path.join(root, name))
    return lst

            
def SendFiles(sender,recipients,
         host,username,password,
         subject,text,textfile,allFiles):
    print ' '
    for f in allFiles:
        if not os.path.isfile(f):
            allFiles.remove(f)
    if len(allFiles)<1:
        print 'No regular file to sent'
        sys.exit(2)
        
    print 'regular file to sent :',allFiles
    print 'sender :',sender
    print 'recipients :',recipients
    print 'host :',host
    print 'username :',username
    print 'password :',
    print '*'*len(password) #password
    print 'subject :',subject
    print ' '

    starttime =PrintStartTime()
    composed =Files2MimeFormat(sender,recipients,
         host,username,password,
         subject,text,textfile,allFiles)
    if composed:
        SendEmail(sender, recipients,host,username,password,composed)
    else:
        print 'Pack files error!'
    stoptime =PrintStopTime()
    PrintTotalTime(starttime,stoptime)

def SendDir(sender,recipients,
         host,username,password,
         subject,text,textfile,filenames):
    allFiles =[]
    for eachfile in filenames:
        if os.path.isfile(eachfile):
            allFiles.append(eachfile)
        elif os.path.isdir(eachfile):
            lst =ListDirFiles(eachfile)
            if lst and len(lst)>0:
                allFiles.extend(lst)
        else :
            print eachfile+' is not regular files(dirtories)!'
    SendFiles(sender,recipients,
            host,username,password,subject,text,textfile,allFiles)

if __name__ == '__main__':
    parser = optparse.OptionParser(usage=usage_info)

    parser.add_option('-f', '--filename',
                      type='string', action='append', metavar='FILENAME',
                      default=[], dest='filenames',
                      help='The file to send (at least one required)')
    parser.add_option('-s', '--sender',
                      type='string', action='store', metavar='SENDER',
                      help='The value of the From: header (required)')
    parser.add_option('-r', '--recipient',
                      type='string', action='append', metavar='RECIPIENT',
                      default=[], dest='recipients',
                      help='A To: header value (at least one required)')
    parser.add_option('-o', '--host',
                      type='string', action='store', metavar='HOST',
                      help='The STMP host.(required)')
    parser.add_option('-u', '--username',
                      type='string', action='store', metavar='USERNAME',
                      help='The username.(required)')
    parser.add_option('-p', '--password',
                      type='string', action='store', metavar='PASSWORD',
                      help='The password.(required)')
    parser.add_option('-t', '--text',
                      type='string', action='store', metavar='TEXT',
                      help='The text.(required)')
    parser.add_option('-T', '--textfile',
                      type='string', action='store', metavar='TEXTFILE',
                      help='The text file.(required)')
    parser.add_option('-S', '--subject',
                      type='string', action='store', metavar='SUBJECT',
                      help='The subject.(required)')

    opts, args = parser.parse_args()

    filenames =opts.filenames
    sender =opts.sender
    recipients =opts.recipients
    host =opts.host
    username =opts.username
    password =opts.password
    text =opts.text
    textfile =opts.textfile
    subject =opts.subject
    
    missing = False
    if not filenames : 
        print 'Missing filenames'
        missing = True
    if not sender : 
        print 'Missing sender'
        missing = True
    if not recipients : 
        print 'Missing recipients'
        missing = True
    if not host : 
        print 'Missing host'
        missing = True
    if not username : 
        print 'Missing username'
        missing = True
    if not missing and not password : 
        password =getpass.getpass('Please input password of your email: ')
    if not text : 
        print 'Missing text'
    if not textfile : 
        print 'Missing textfile'
    if not subject : 
        print 'Missing subject,use default.'
        subject ="Sent from sm.py,content is "+str(filenames)
            
    if missing :
        parser.print_help()
        sys.exit(1)

    SendDir(sender,recipients,
         host,username,password,
         subject,text,textfile,filenames)
