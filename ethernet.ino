#include <EtherCard.h>

String Temperature = "12";
String VitesseVent = "200";

String data = String("index.php?g=") + Temperature + String("&u=") + VitesseVent;

char __data[100];


// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
static uint32_t timer;

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void setup () {
  
  data.toCharArray(__data, sizeof(__data));
  Serial.begin(9600);
  Serial.println(F("\n[webClient]"));
  Serial.println(__data);

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  char websiteIP[] = "192.168.0.13";
  ether.parseIp(ether.hisip, websiteIP);
    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer) {
    timer = millis() + 5000;
    Serial.println();
    Serial.print("<<< REQ ");
    ether.browseUrl(PSTR("/ethernetcard/"), __data, "192.168.0.13", my_callback);
  }
}
