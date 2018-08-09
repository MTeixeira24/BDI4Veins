FROM openjdk:10

COPY ./ /usr/share/src/
WORKDIR /usr/share/src/

RUN apt-get update -y && apt-get upgrade -y
RUN apt-get install -y  maven

RUN mvn package 

CMD java -jar target/JVeins-1.0-SNAPSHOT.jar 
