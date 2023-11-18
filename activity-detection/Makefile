.PHONY: install-lib samples

LIB_INSTALL_DIR = ./libs

install-lib:
	@mkdir -p $(LIB_INSTALL_DIR)
	@pip install -r requirements.txt -t $(LIB_INSTALL_DIR)

lambda.zip:
	zip -r lambda.zip ./lambda_function.py ./libs

zip: lambda.zip

samples:
	./tools/make_samples.sh samples/mybath.wav