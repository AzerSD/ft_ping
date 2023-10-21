FROM ubuntu:latest

CMD ["/bin/bash"]

RUN rm /etc/dpkg/dpkg.cfg.d/excludes

RUN apt update \
	&& apt install -y gcc gdb make iputils-ping tcpdump ranger vim\
	&& mkdir /root/ft_ping

WORKDIR /root/ft_ping