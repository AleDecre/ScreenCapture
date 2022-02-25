# Documentazione libreria ScreenRecorder
## ***Classe ScreenRecorder***
&nbsp;

## Costruttore
### ScreenRecorder(int audio)
#### Costruttore della classe ScreenRecorder.

|Parametro| Descrizione | 
|---| ---|
|***int*** audio| Se = 1 inizializza l'oggetto per la registrazione audio e video, se = 0 inizializza l'oggetto per la registrazione solo video.|

>**Esempi:**
>
>- ScreenRecorder *scAudioVideo = new ScreenRecorder(1);
>- ScreenRecorder *scVideo = new ScreenRecorder(0);

&nbsp;\
&nbsp;

## Metodi per il file di output
### ***int*** init_outputfile_AV(string file)
#### Inizializza la struttura dati (AVFormatContext) per il file di output.

|Parametro| Descrizione | 
|---| ---|
|***string*** file| Nome del path completo del file di output.|

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore|

>**Esempi:**
>
>- sc->init_outputfile_AV("C:/Users/Andrea/Desktop/video.mp4");

&nbsp;

### ***int*** create_outputfile()
#### Crea un file multimediale vuoto, scrivendone l'header e preparandolo all'acquisizione degli stream audio/video.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore|
|-1| Ritorno con errore sul numero di stream|

>**Esempi:**
>
>- sc->create_outputfile();

&nbsp;

### ***int*** close_outputfile()
#### Scrive il trailer in coda al file di output e lo chiude.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore|

>**Esempi:**
>
>- sc->close_outputfile();

&nbsp;\
&nbsp;

## Metodi per il video

### ***int*** openScreen()
#### Apre l'input device video.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore apertura dell'input device|
|-1| Ritorno con errore|

>**Esempi:**
>
>- sc->openScreen();

&nbsp;

### ***int*** setVideoDecoder()
#### Inizializza e imposta il decoder video. In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|

>**Esempi:**
>
>- sc->setVideoDecoder();

&nbsp;

### ***int*** setVideoEncoder()
#### Inizializza e alloca l'encoder video a partire dal formato del file di output, impostando i parametri ottimali. In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|

>**Esempi:**
>
>- sc->setVideoEncoder();

&nbsp;

### ***int*** recordVideo()
#### Inizia l'acquisizione di pacchetti video per un tempo indefinito. Per gestire la terminazione bisogna invocare la funzione all'interno di un thread e agire sul mutex `stop_m` per modificare la variabile condivisa (flag) `stop`. In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore nel processare il pacchetto|

>**Esempi:**
>
> *Da usare all'interno di un thread*
>- sc->recordVideo();

>*Interruzione acquisizione:*
>
>- sc->stop_m.lock(); \
> sc->stop = 1; \
> sc->stop_m.unlock();

&nbsp;

### ***int*** closeVideo()
#### Chiude l'input video e libera la struttura dati relativa (AVFormatContext). In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|

>**Esempi:**
>
>- sc->closeVideo();

&nbsp; \
&nbsp;

## Metodi per l'audio

### ***int*** openMic()
#### Apre l'input device audio.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore apertura dell'input device|
|-1| Ritorno con errore|

>**Esempi:**
>
>- sc->openMic();

&nbsp;

### ***int*** setAudioDecoder()
#### Inizializza e imposta il decoder audio. In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|

>**Esempi:**
>
>- sc->setAudioDecoder();

&nbsp;

### ***int*** setAudioEncoder()
#### Inizializza e alloca l'encoder audio a partire dal formato del file di output, impostando i parametri ottimali. In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|

>**Esempi:**
>
>- sc->setAudioEncoder();

&nbsp;

### ***int*** recordAudio()
#### Inizia l'acquisizione di pacchetti audio per un tempo indefinito. Per gestire la terminazione bisogna invocare la funzione all'interno di un thread e agire sul mutex `stop_m` per modificare la variabile condivisa (flag) `stop`. In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|
|1| Ritorno con errore nel processare il pacchetto|

>**Esempi:**
>
> *Da usare all'interno di un thread*
>- sc->recordVideo();

>*Interruzione acquisizione:*
>
>- sc->stop_m.lock(); \
> sc->stop = 1; \
> sc->stop_m.unlock();

&nbsp;

### ***int*** closeAudio()
#### Chiude l'input audio e libera la struttura dati relativa (AVFormatContext). In caso di errore non recuperabile, l'applicazione termina forzatamente con un codice di errore.

|Valore di ritorno| Descrizione | 
|---| ---|
|0| Ritorno con successo|

>**Esempi:**
>
>- sc->closeAudio();
