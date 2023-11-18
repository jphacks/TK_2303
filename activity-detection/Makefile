.PHONY: install-lib samples build-image

LIB_INSTALL_DIR = ./libs

install-lib:
	@mkdir -p $(LIB_INSTALL_DIR)
	@pip install -r requirements.txt -t $(LIB_INSTALL_DIR)

lambda.zip:
	zip -r lambda.zip ./lambda_function.py ./libs

zip: lambda.zip

samples:
	./tools/make_samples.sh samples/mybath.wav

build-image:
	docker build --build-arg ARCH=arm64 -t vad-lambda .

run:
	docker run -p 9000:8080 vad-lambda:latest
