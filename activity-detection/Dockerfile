FROM public.ecr.aws/lambda/python:3.11

# ARG ARCH

RUN yum update -y
RUN yum install -y gcc
RUN yum install -y tar
RUN yum install -y xz
# RUN yum install libsndfile-devel -y
RUN yum install -y bash

WORKDIR /build

CMD ["/bin/bash"]
