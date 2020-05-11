#include <SPI.h>
#include <MFRC522.h>

int buzzerPin = 2; // buzzer pini
int notaSayisi = 3; //buzzer notaları
int C = 11;
int D = 1000;
int E = 300;
int F = 11;
int G = 250;
int A = 400;
int notalar[] = {C, D, E};
int notalar2[] = {F, G, A};

#define RST_PIN         9           // Rfid okuyuzu
#define SS_PIN          10          // Rfid okuyuzu

MFRC522 mfrc522(SS_PIN, RST_PIN);   //  MFRC522 Oluşturma

MFRC522::MIFARE_Key key; // KEY oluşturma

/**
 * Setup.
 */
void setup() {

  
    pinMode(8, OUTPUT);// LEDLER
    pinMode(7, OUTPUT);// LEDLER
  
    
    Serial.begin(9600); // Serial
    while (!Serial);    
    SPI.begin();        //  SPI bus
    mfrc522.PCD_Init(); //  MFRC522 kart

    // Keyler 
    // Sıfıra Çevirme İşlemi
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Lutfen Tarama Yapın"));
    Serial.print(F("A ve B Keyleri:"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();

    Serial.println(F("DİKKAT: Veriler PICC'ye sektör 1'de yazılacak"));
}

/**
 * Main loop.
 */
void loop() {
  
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

   
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Kartın Detayları
    Serial.print(F("Kart UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC tipi: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Kartın uyumluluk kontrolü
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Bu örnek yalnızca MIFARE Classic kartlarla çalışır."));
        return;
    }


    // Bu örnekte ikinci sektörü kullanıyoruz,
    // yani: sektör # 1, blok # 4'e kadar ve blok # 7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
        0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
    };
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    //  A key ile kimlik doğrulaması
    Serial.println(F("A Keyi ile kimlik doğrulama..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Şuan ki verileri gösterme
    Serial.println(F("Şuan ki veriler"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Bloktaki veriyi okuma
    Serial.print(F("Blok içerisindeki veri okunuyor ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Blok içindeki veriler ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();

    //  B key ile kimlik doğrulaması
    Serial.println(F("B Keyi ile kimlik doğrulama..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
      // Bloktaki verileri okuma
    Serial.print(F("Bloktaki veriler okunuyor")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() başarısız: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Bloktaki veriler ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    // Bloktaki verilerin yazdıklarımız olduğunu kontrol edilmesi
    Serial.println(F("Sonuçlar kontrol ediliyor..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
       
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Eşleşen bytelar = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Başarılı :-)"));
          Serial.println("Kapi acildi");
    
        
       digitalWrite(8, HIGH); // Ledler Yanıyor
      delay(3000);
      digitalWrite(8, LOW);
            for (int i = 0; i < notaSayisi; i++) // Buzzerdan ses
  {
    tone(buzzerPin, notalar[i]);
    delay(500);
    noTone(buzzerPin);
    delay(20);
  }
  noTone(buzzerPin);  
      
   setup();
        
    } 
    
    else {
        Serial.println(F("Başarısız, eşleşme yok :-("));
       Serial.println(F("Yetkisiz Kart"));

   
  
      digitalWrite(7, HIGH); // LED Yanması
      delay(3000);
      digitalWrite(7, LOW);
       for (int i = 0; i < notaSayisi; i++) // Buzzer sesi
  {
    tone(buzzerPin, notalar[i]);
    delay(500);
    noTone(buzzerPin);
    delay(20);
  }
  noTone(buzzerPin);  
          Serial.println(F("Kopyalamak için 1'e basın")); // Kopyalama işlemi
      
   char choice;

       choice = Serial.read();
  delay(5000);
       
   
       if (choice == '1' ){
    Serial.print(F(" Veri bloklara yazılıyor ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() başarısız: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();
       }
  else {
    loop();}
       

 

    
    Serial.println();

   
    Serial.println(F("Sektördeki veriler:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    
    mfrc522.PICC_HaltA();
    
    mfrc522.PCD_StopCrypto1();
}
}
/**
 * Seriye onaltılık değerler olarak bir bayt dizisi dökmek için yardımcı rutin..
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
