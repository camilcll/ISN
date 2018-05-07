
#include <SoftwareSerial.h>
#include <EtherCard.h>
#include <math.h>




// adress MAC du module
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700]; // definition de l'espace de stockage du buffer
static uint32_t timer;

// print la requette lorsque la requette est terminee
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

const char* demandearray[] = {"G","T","P","A","H"};

SoftwareSerial stationserial(9, 10, true); // (RX, TX) (pin Rx station, pin Tx station)
byte b1 ;
byte b2 ;

byte Bit_Reverse(byte Monbyte) { // fonction d'inversion d'octet
  byte  rtn = 0;
  for (byte i=0; i<8; i++) {
    bitWrite(rtn, 7-i, bitRead(Monbyte, i));
  }
  return rtn;
}


void setup()
{
    // Ouvre la voie série avec l'ordinateur
    Serial.begin(9600);  
    // Ouvre la voie série avec la station
    stationserial.begin(4800);
   
   if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) // test pour savoir si carte est branchee
     Serial.println(F("Probleme d'acces a la carte ethernet"));
   if (!ether.dhcpSetup()) // test pour savoir si serveur dhcp est existant
      Serial.println(F("erreur DHCP"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  char websiteIP[] = "192.168.0.13"; // ardesse IP qui recoit la requette
  ether.parseIp(ether.hisip, websiteIP);
    
  ether.printIp("SRV: ", ether.hisip);
}

void comm(const char* var) // fonction qui etabli la communication
{
   stationserial.print("$");
   delay(30);
   stationserial.write(var); 
}

void loop()
{
   int Tt;
   int Pmbar;
   int Hr;
   String girposition;
   int Va;
   for (int i = 0; i < 5; i++){ 
    
    
   
    comm(demandearray[i]);
    
    if (stationserial.available()) {
      const char* gir = "G";
      const char* ane = "A";
      const char* temp = "T";
      const char* hum = "H";
      const char* pres = "P";
      
      if (demandearray[i] != gir and demandearray[i] != ane){ // test pour savoir si utilisation du CAN est necessaire
        b1 = stationserial.read();
        b2 = stationserial.read();
        
        while (stationserial.available()) stationserial.read();

        Serial.print(b1);
        Serial.print(" - ");
        Serial.print(b2);
        Serial.println();
        
        b1 = Bit_Reverse(b1);
        b2 = b2 & byte(192);
        b2 = Bit_Reverse(b2);
        int b = b2*256;
        int bfinal = b1+b;
        Serial.println(bfinal);
        
        if(demandearray[i] = temp){
          int TUr = (5/1024)*bfinal;
          int TRt = round((10000*TUr)/(5-TUr));
          int Tt =  (3900/(log(TRt/10000)+(3900)/(25+273)))-273;
          
        }
        else if(demandearray[i] = pres){
          int PUr = (5/1024)*bfinal;
          int Pmbar = round((((PUr-(5*(-0.1)))/(5*(0.01)))*10));   
      }
        else if(demandearray[i] = hum){
          int HUr = (5/1024)*bfinal;
          int HRt = round(50000*exp((3900/(Tt+273))-(3900/(25+273))));
          int HRh = round((HRt/HUr)*(5-HUr));
          int Hr = round((abs(HRt-HRh)/HRh)*5.6*100);
        
      }
      else if(demandearray[i] = gir){
          byte b = stationserial.read();
          Serial.write(b);
          if(b == "@"){
            String girposition = "NORD";
          }
          else if(b == "B"){
            String girposition = "NORD-EST";
          }
          else if(b == "J"){
            String girposition = "EST";
          }
          else if(b == "F"){
            String girposition = "SUD-EST";
          }
          else if(b == "D"){
            String girposition = "SUD";
          }
          else if(b == "L"){
            String girposition = "OUEST";
          }
          else if(b == "H"){
            String girposition = "NORD-OUEST";
          }
          else if(b == "N"){
            String girposition = "SUD-OUEST";
          }
        }
      }
      else if(demandearray[i] = ane){
        byte a = stationserial.read();
        int Va = (50/1000)*a*2*PI;
      }
    }
       

   }
     
     ether.packetLoop(ether.packetReceive());
     String data = String("index.php?T=") + String (Tt) + String("&P=")+ String(Pmbar)+ String("&A=")+ String(Va)+ String("&H=")+ String(Hr)+ String("&G=")+ String(girposition);

     char __data[100];
     data.toCharArray(__data, sizeof(__data));
     Serial.begin(9600);
     Serial.println(F("\n[webClient]"));
     Serial.println(__data);
  
  if (millis() > timer) { // boucle qui envoie les donnees toutes les 10 minutes
    timer = millis() + 600000; 
    Serial.println();
    Serial.print("<<< REQ ");
    ether.browseUrl(PSTR("/ethernetcard/"), __data, "192.168.0.13", my_callback);
}

}

