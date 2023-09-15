FROM debian:bullseye	

EXPOSE 443

RUN apt update && apt upgrade -y && apt install -y \ 
    nginx \
    python3 \
    python3-pip \
    fcgiwrap \
    dos2unix \
    && rm -r /etc/nginx/sites-enabled && rm -r /etc/nginx/sites-available 
   
COPY ./nginx.conf ./etc/nginx/conf.d/
COPY ./pages/ /var/www/html
COPY ./cgi-bin /var/www/cgi-bin
COPY ./script.sh .

RUN mkdir /var/www/images && chown -R www-data:www-data /var/www/ \
    && chmod 777 /var/www/cgi-bin && chmod 777 /var/www/cgi-bin/test.sh && chmod 777 /var/www/cgi-bin/upload.py\
    && dos2unix /var/www/cgi-bin/upload.py && dos2unix /var/www/cgi-bin/test.sh
    
ENTRYPOINT [ "bash", "script.sh" ]
