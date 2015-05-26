#!/usr/bin/env python
#
# NOT TESTED when adding this macro
#

import sys
import os
import smtplib
import base64

SMTP_SERVER = 'smtp.gmail.com'
SMTP_PORT = 587
sender =    'gpenghe@gmail.com'
recipient = 'gengp1@utrc.utc.com'
password =  'googlegg31465'  
 
if len(sys.argv) == 1:
    print "Usage: ./sendmail.py <file>"
    exit()

def getmsg1():
    # Read a file and encode it into base64 format
    filename = sys.argv[1]
    fo = open(filename, "rb")
    basename = os.path.basename(filename)
    
    filecontent = fo.read()
    encodedcontent = base64.b64encode(filecontent)  # base64

    marker = "AUNIQUEMARKER"

    body ="""
    This is a test email to send an attachement.
    """
    # Define the main headers.
    part1 = """From: From Person <me@fromdomain.net>
To: To Person <gpenghe@gmail.com>
Subject: Sending Attachement
MIME-Version: 1.0
Content-Type: multipart/mixed; boundary=%s
--%s
""" % (marker, marker)

    # Define the message action
    part2 = """Content-Type: text/plain
Content-Transfer-Encoding:8bit

%s
--%s
""" % (body,marker)

    # Define the attachment section
    part3 = """Content-Type: multipart/mixed; name=\"%s\"
Content-Transfer-Encoding:base64
Content-Disposition: attachment; filename=%s

%s
--%s--
""" %(basename, basename, encodedcontent, marker)

    message = part1 + part2 + part3
    return message

def getmsg2():
    subject = 'Gmail SMTP Test'
    body = 'blah blah blah 2'

    "Sends an e-mail to the specified recipient."
    body = "" + body + ""
    headers = ["From: " + sender,
           "Subject: " + subject,
           "To: " + recipient,
           "MIME-Version: 1.0",
           "Content-Type: text/html"]
    headers = "\r\n".join(headers)

    return headers + "\r\n\r\n" + body;

message = getmsg1()
try:
    session = smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
    session.starttls()  
    session.login(sender,password)  
    session.sendmail(sender, recipient, message)
    session.quit()
    print "Successfully sent email"
except Exception:
    print "Error: unable to send email"
