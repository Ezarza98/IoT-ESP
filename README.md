# IoT-ESP
Set of codes that allow commands to be sent to ESP-type boards through voice commands with Alexa. //  Conjunto de códigos que permiten mandar comandos a las placas del tipo ESP a través de comandos de voz con Alexa.

The Code for Lambda must be used in AWS Lambda to receive the different instruction from Alexa and interpret them.

The code for Alexa Skill could be copied in the .json archive in the menu for your Skill from ALexa Skill Kit.

The Arduino Code is for Arduino Mega2560 but you can use it with another type of board taking care with the name of the pin for Serial Port and Digital Pin.

To use the code for the different ESP Board you must create a certificate for each board you want to use. You need 3 certificates, a private Key and a public key you can obtein from Amazon searching private.pem for private key, AmazonrootCA1 for public Key and a third certificate that you must crate using AWS IoT. You need to save the three certificates with extension .der, so you must convert them. I recommend using OpenSSL.

The certificates need to have the name: "cert.der" for the certificate from AWS IoT. "ca.der" for the certificate AmazonrootCA1. "private.der"  for the private key. 
You need to save the certificates in a folder named "data". In the same folder you save "data" save the main program. When you open the code, connect your board and submit the code.
