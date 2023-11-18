FROM public.ecr.aws/lambda/python:3.11

ARG ARCH


RUN yum update -y
RUN yum install -y gcc
RUN yum install -y tar
RUN yum install -y xz
RUN yum install libsndfile-devel -y

# install ffmpeg
WORKDIR /tmp
RUN curl -O https://johnvansickle.com/ffmpeg/releases/ffmpeg-release-${ARCH}-static.tar.xz
RUN tar xvf ffmpeg-release-${ARCH}-static.tar.xz
RUN mv ffmpeg-6.1-${ARCH}-static/ffmpeg /usr/local/bin/ffmpeg
RUN mv ffmpeg-6.1-${ARCH}-static/ffprobe /usr/local/bin/ffprobe
RUN rm -rf ffmpeg-6.1-${ARCH}-static

WORKDIR /var/task
COPY requirements.txt ./
RUN pip install -r requirements.txt
COPY lambda_function.py ./

CMD ["lambda_function.lambda_handler"]
