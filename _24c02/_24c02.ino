//*********************************************************************************************************************************************
//Software scritto con Energia versione 0101E0009 per Texas Instrument LaunchPad MSP430G2 con MSP430G2553
//
//SCRITTORE DI DATI IN SPECIFICI INDIRIZZI PER EEPROM 24c02.
//
//Il programma:
//              1) scrive i dati contenuti nell' array cod_1 negli indirizzi addr1 e seguenti
//              2) scrive i dati contenuti nell' array cod_2 negli indirizzi addr2 e seguenti
//              3) controlla i dati scritti in addr1 e nei seguenti "len" indirizzi
//              4) controlla i dati scritti in addr2 e nei seguenti "len" indirizzi
//              5) manda i dati letti su porta seriale
//              6) se i dati sono corretti (flag0 e flag1 = 0) led rosso acceso
//              7) se i dati non sono corretti o non è avvenuta la lettura (flag0 o flag1 = 1) led rosso lampeggiante
//
//AUTORE:_________________________________________________________________________________________________
//Nome: Davide Florini                                                                                    |
//Email: Davideflorini92@gmail.com                                                                        |
//________________________________________________________________________________________________________|
//
//info:___________________________________________________________________________________________________
//    HARDWARE:                                                                                           |
// I2C-bus specification and user manual: http://www.nxp.com/documents/user_manual/UM10204.pdf ;          |
// TEXAS INSTRUMENT LaunchPad MSP430G2: http://www.ti.com/tool/msp-exp430g2 ;                             |
// TEXAS INSTRUMENT MSP430G2553: http://www.ti.com/product/msp430g2553 ;                                  |
// 24c02:http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en010798 ;                            |
//   SOFTWARE:                                                                                            |
// Energia: https://github.com/energia/Energia/wiki/Getting-Started ;                                     |
//________________________________________________________________________________________________________|
//
//CIRCUITO:_______________________________________________________________________________________________
//                                                                                                        |
//                                                                                                        |
//                                                                                                        |
//                                         ^     ^ vcc=3,55V                                              |
//               _______________           |     |                                                        |
//              |               |         | |   | |                                                       |
//              |               |         | |R  | |R=2,5K (circa)                                         |
//              |  MSP430G2553  |         |_|   |_|                                                       |
//              |               |          |     |                               ______________           |
//              |               |P1.6 <----+---- | --SDA---------------------->5|              |          |
//              |               |P1.7 -----------+---SCL---------------------->6|              |          |
//              |               |P2.5 ---------------GND-------------->1,2,3,4,7|     24c02    |          |
//              |               |P2.4 ---------------VCC---------------------->8|              |          |
//              |_______________|                                               |______________|          |
//                                                                                                        |
//________________________________________________________________________________________________________|
//
//
//Binary sketch size: 3.701 bytes (of a 16.384 byte maximum)
//
//
//*********************************************************************************************************************************************


#include <Wire.h>                           //libreria per l'utilizzo del protocollo I2C
byte addr1=0x00, addr2=0x08 ;               //Indirizzi della memoria in cui scrivere i dati
int len=4;                                  //lunghezza dati da scrivere (leggere)
byte read_val=0;                            //valore letto
byte cod_1[4]={0x0C, 0x01, 0x0A, 0x00};     //CODICI DA SCRIVERE (E CONTROLLARE) IN addr1, la lunghezza dell'array deve essere uguale a len
byte cod_2[4]={0x0C, 0x01, 0x0A, 0x00};     //CODICI DA SCRIVERE (E CONTROLLARE) IN addr2, la lunghezza dell'array deve essere uguale a len
char i=0;                                   //
boolean flag0=0, flag1=0;                   //i flag sono utilizzati per il controllo dei byte (vedi)
void setup (void)
{
  Serial.begin (9600);                     //inizializza l'UART a 9600 baud
  Wire.begin();                            //inizializza l'I2C
  pinMode(P1_0, OUTPUT);
  pinMode(P1_3, INPUT);
  pinMode(P2_4, OUTPUT);
  pinMode(P2_5, OUTPUT);
  digitalWrite(P2_4, HIGH);                //vcc per eeprom
  digitalWrite(P2_5, LOW);                 //gnd per eeprom
}

void loop ()
{
  
//*********************************************************************************************************************************************
// Fase iniziale, tenere premuto P1_3 per proseguire (è necessario tenerlo premuto a lungo, massimo circa un sec)                             *
//*********************************************************************************************************************************************

    while (digitalRead (P1_3) == 1)
  {
    digitalWrite (P1_0, HIGH);
    delay(500);
    digitalWrite (P1_0, LOW);
    delay(500);
  }

//*********************************************************************************************************************************************
// SCRITTURA BYTE                                                                                                                             *
//*********************************************************************************************************************************************  

//*********************************************************************************************************************************************
// Scrittura dei byte definiti in cod1[]                                                                                                      *
//*********************************************************************************************************************************************  
  
  Wire.beginTransmission (0x50);    //inizia la trasmissione I2C per scrivere sul dispositivo 0x50 (0b1010000). R/_W = 0
  Wire.write((byte)addr1);          //manda l'indirizzo da cui si vuole partire per scrivere i byte
  delay(5);                         //piccolo ritardo
  while (i < len)                   //ripeti fino a quando tutti i byte non sono scritti
    {
      Wire.write((byte)cod_1[i]);   //scrivi cod[i]
      delay (5);                    
      i++;                          //incrementa i
    }
  Wire.endTransmission();           //manda il segnale di stop
  
  i=0;                              //resetta il puntatore i
  
//*********************************************************************************************************************************************
// Scrittura dei byte definiti in cod2[]                                                                                                      *
//*********************************************************************************************************************************************  

  Wire.beginTransmission (0x50);    //vedi scrittura in addr1
  Wire.write((byte)addr2);
  delay(5);
  while (i < len)
    {
      Wire.write((byte) cod_2[i]);
      delay (5);
      i++;
    }
  Wire.endTransmission();

  i=0;        //ripristina i
  
//*********************************************************************************************************************************************
//LETTURA DEI BYTE SCRITTI E CONTROLLO                                                                                                        *
//*********************************************************************************************************************************************  
  delay(10);
//*********************************************************************************************************************************************
// Lettura byte in addr1, se i valori sono diversi da quelli definiti in cod1[] viene portato a 1 il flag0                                    *
//*********************************************************************************************************************************************  

  Serial.print (addr1, HEX);         //
  Serial.print (":    ");            //Stampa su seriale il valore di addr1
  
  Wire.beginTransmission (0x50);     //inizia la trasmissione con 0x50. R/_W = 0
  Wire.write((byte)addr1);           //invia l'indirizzo da cui iniziare a leggere
  Wire.endTransmission();            //stop trasmissione
  Wire.requestFrom(0x50,len);        //invia 0x50 (0b1010000), R/_W = 1. Ricevi len byte da 0x50
  delay(100);                        //aspetta che tutti i byte vengano ricevuti

  for(i=0 ; i < len ; i++)            //ripeti len volte
   {
    read_val = Wire.read ();          //assegna a read_val il valore del primo byte in memoria, una volta letto il dato 
                                      //si cancella e richiamando Wire.read() viene letto il byte dopo
    Serial.print(read_val, HEX);      //scrivi su seriale i "len" valori salvati dall'indirizzo addr1 in poi 
    Serial.print("    ");             //distanziali
    if (read_val != cod_1[i]) flag0=1; //se un valore letto è diverso dal valore definito flag=1
    delay (5);                        //piccolo ritardo
   }
   
  Serial.println ("");                  //nuova riga

  i=0;                                  //Ripristina i

//*********************************************************************************************************************************************
// Lettura byte in addr2, se i valori sono diversi da quelli definiti in cod2[] viene portato a 1 il flag1                                    *
//*********************************************************************************************************************************************  

  Serial.print (addr2, HEX);         //
  Serial.print (":    ");            //Stampa su seriale il valore di addr2

  Wire.beginTransmission (0x50);
  Wire.write((byte)addr2);
  Wire.endTransmission();
  Wire.requestFrom(0x50,4);
  delay(10);

  for(i=0; i<len; i++)
   {
    read_val = Wire.read();
    Serial.print (read_val, HEX);      //scrivi su seriale i "len" valori salvati dall'indirizzo addr1 in poi
    Serial.print ("    ");             //distanziali
    if ( read_val!= cod_2[i]) flag1=1;
    delay (5);
   }

  Serial.println("");                   //nuova riga

//*********************************************************************************************************************************************
// CONTROLLO DEI FLAG                                                                                                                         *
//*********************************************************************************************************************************************  


//*********************************************************************************************************************************************
// Se i flag sono entrambi a 0 (tutti i valori letti sono uguali ai valori definiti) accendi il LED collegato a P1_0.                         *
// Necessario reset per riiniziare                                                                                                            *
//********************************************************************************************************************************************* 

 
 if (flag0==0 && flag1==0)
 {
   while(1)
   {
    digitalWrite (P1_0, HIGH);
   }
 }
 
//*********************************************************************************************************************************************
// Se anche un solo flag è diverso 0 (non tutti i valori letti sono uguali ai valori definiti) lampeggia LED collegato a P1_0.                *
// Necessario reset per riiniziare                                                                                                            *
//********************************************************************************************************************************************* 

 else
 {
   while(1)
   {
    digitalWrite (P1_0, HIGH);
    delay(500);
    digitalWrite (P1_0, LOW);
    delay(500);
   }
 }
/*********************************************************************************************************************************************/
}
