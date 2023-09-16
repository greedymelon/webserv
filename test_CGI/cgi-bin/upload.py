#!/usr/bin/python3

import cgi
import cgitb; cgitb.enable()
import os
import random
import string
import sys
import errno

print("Content-type:text/html\r\n\r\n", end="")

dir_path = "/var/www/images/"
files = os.listdir(dir_path)

base_begin = """
<!DOCTYPE html>
<html lang="en">
<head>
    
	<link rel="stylesheet" href="/index.css">
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>WebServ</title>

    <!-- Including jQuery -->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.js">
    </script>

    <!-- Including Bootstrap JS -->
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js">
    </script>
</head>
<body>
"""

base_middle = """
 	<div class="cgi-screen">
 		<div>
<form enctype = "multipart/form-data" action = "/cgi-bin/upload.py" method = "POST">
   <p>File: <input type = "file" name = "fileName" /></p>
   <p><input type = "submit" value = "Upload" /></p>
   </form>
   </div>
"""
base_end = """
<h2 class="statusText">Return <a href="/index.html">home?</a></h2>
	  </div>
</body>
</html>
"""
def base_page(message="none", type=0):
    print (base_begin)
    if message != "none":
        if type == 1:
            print ('<div id="alert" class="alert alert-danger">')
        else:
            print ('<div id="alert" class="alert alert-success">')
        print(message + '</div>')
        print ('<script type="text/javascript">') 
        print ('setTimeout(function () {')
        print ("$('#alert').alert('close');")
        print ("}, 5000);")
        print ('</script>')
    print (base_middle)
    files = os.listdir(dir_path)
    if files:
        print ('<h1 class="cgiTitle">Uploaded Images</h1>')
        print ('<div class="uploadedImage">')
        for file in files:
            print ('<div class="uploadedImageItem">')
            print ('<img src="'+ "/images/" + file + '" alt="' + file + '" class="uploadedImageSrc">')
            print ("<form enctype = \"multipart/form-data\" action = \"/cgi-bin/upload.py\" method = \"post\">")
            print ("<input type=\"hidden\" name=\"delete\" value=\"" + file + "\">")
            print ("<input type = \"submit\" value = \"delete\" />")
            print ("</form>")
            print ('</div>')
        print ('</div>')
    print (base_end)

def upload_image():
    form = cgi.FieldStorage()
    if "delete" in form:
        delete_image(form)
        return
    if "fileName" not in form:
        base_page("You need to provide a file to upload a file", 1)
        return 
    filename = str(form["fileName"].filename) 
    if '.png' == filename[-4:] or '.jpeg' == filename[-5:]:
        temp_pathname = dir_path + filename
        while True:
            if os.path.exists(temp_pathname):
                file_dtls = temp_pathname.split('.')
                temp_pathname = file_dtls[0] + random.choice(string.ascii_letters) + '.' + file_dtls[1]
            else:
                break
      
        fileitem = form["fileName"]
        open(temp_pathname, "wb").write(fileitem.file.read())
        base_page(filename + " was uploaded successfully")
    else:
        base_page("Wrong format" + filename, 1)

def delete_image(form):
    if not form["delete"].value:
        base_page("You need to provide a file to delete", 1)
        return
    filename = str(form["delete"].value)
    try:
        temp_pathname = dir_path + filename
        os.path.exists(temp_pathname)
        os.remove( temp_pathname)
        base_page(filename + " was correctly deleted")
    except FileExistsError:
        base_page("The file you want to delete does not exist", 1)
          
method_map = {"GET": base_page,
"POST": upload_image,
}
method_map[os.environ['REQUEST_METHOD']]()