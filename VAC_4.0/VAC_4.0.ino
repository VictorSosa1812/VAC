int pwm=10;
int T_Const=2, estado =0, pin_buzz= 5,pin_can=6, Niv_Liq=7 ;
int T_Var=3;
int START = 4;
float tiempo_limite_bomba=1000;
float dif_temp;
int pin_PWM_bomba = 9;
int pot= A0,pin_Bat=A1, presion= A2;
float t0;
int est_pot, R, Ter_Act,C,V, S, bateria, i, C_I, C_N, niv_pre, P;
//tenemos que sacar una relación para cada pwm, con el tubo
//obstaculizado y así obtener una variable: valor__bloqueo
int valor_bloqueo=14;
int analog_in =A3;
float val=0;
float voltaje =0;
float volt =0;
float voltaje_real;
int j;
int R1=10000,R2=2000;
float volt_fuente_max=18;
float Bat;

int analog_in_mot =A4;
float val_mot=0;
float voltaje_mot =0;
float volt_mot =0;
float voltaje_real_mot;
int k;
int R3=10000,R4=2000;
int m=0;
void setup() {
  Serial.begin(9600);
  pinMode(T_Const, INPUT);
  pinMode(T_Var, INPUT);

}

void loop() {
fsm_VAC();
}
void fsm_VAC()
{
  switch (estado)
  {
    case 0:
    Serial.println("estado 0");    
    est_pot = analogRead(pot);
    R = map(est_pot,0,1024, 55,115);
    C= digitalRead(T_Const);
    V= digitalRead(T_Var);
    m=0;
    if ( (C+V)!=0)
    {
      estado = 1;

    }
    Serial.println(R);
    
    break;
    
    case 1:
    Serial.println("estado 1"); 
    if (C==1)
    {
      Ter_Act=C;
      Serial.println("Terapia elegida: CONSTANTE"); 

    }
    else
    {
      Ter_Act=V;
      Serial.println("Terapia elegida: VARIABLE"); 
    }
    
    estado=2; 
    break;
    
    case 2:
    Serial.println("estado 2");
    S=digitalRead(START);
    if (S != 1)
    {
      estado = 0;
    }
    else
    {
      estado = 3;
      Serial.println("START"); 
    }
    
    break;
    
    case 3:
    Serial.println("estado 3");
    Serial.println("Revision del Sistema");
    estado = 4;
    break;
    
    case 4:
    i=1; //servirá para el momento de la falla vaya a ejecucion de alarmas
    Serial.println("estado 4");
    Serial.println("Evaluacion de la Bateria");
    //estas 2 línes fueron para simular con el tercer potenciómetro
    //bateria = analogRead(pin_Bat);
    //Bat = map(bateria,0,1024, 0,100);
    voltaje_bat();//para simular la variación de la batería con el potenciómetro 2,
                  //activar las 2 lineas de arriba y comentar este:voltaje_bat();
    
    if (Bat <= 10)
    {
      Serial.println("Bateria demasiado baja");
      estado = 7;
    }
    else if (Bat >10 && Bat <=20)
    {
      Serial.println("Bateria entre 10% y 20%, debe cargarlo");
      estado = 5;
    }
    else {
      Serial.println("Bateria bien cargada");
      estado = 6;
    }
    break;
    
    case 5:
    Serial.println("estado 5");
    tone(pin_buzz, 300,10);
    estado = 6;
    break;
    
    case 6:
    i=2;
    Serial.println("estado 6");
    Serial.println("Analizar colocacion de canister");
    noTone(pin_buzz);
    C_I = digitalRead(pin_can);
    if (C_I == 1)
    {
      estado = 8;
    }
    else{
      estado = 7;
    }
    break;
    
    case 7:
    Serial.println("estado 7");
    Serial.println("Ejecucion de alarmas dependiendo de i(tipo de problema)");

    tone(pin_buzz, 300,10);
    //lo que mostrara en la pantalla led que dependa del i----------
    estado= 14;
    m = m+1;
    break;
    
    case 8:
    i=3;
    Serial.println("estado 8");
    Serial.println("Analisis del nivel del liquidos");
    C_N = digitalRead(Niv_Liq);
    if (C_N==1)
    {
      estado = 7;
    }
    else{
      estado =9;
    }
    break;
    
    case 9:
    Serial.println("estado 9");
    Serial.println("Listo 1");
    if (Ter_Act=C)
    {
      estado = 10;
      t0= millis(); //ayudará a medir en cuanto tiempo en el próximo estado la bomba llega a la presión indicada

    }
    else{
      estado= 18;
      t0= millis(); //ayudará a medir en cuanto tiempo en el próximo estado la bomba llega a la presión indicada

    }
    break;
    
    case 10:
    Serial.println("estado 10");
    Serial.println("Lectura del sensor de presion");
    //lectura del sensor de presión, simularemos con un potenciómetro    //lectura del sensor de presión, simularemos con un potenciómetro
    niv_pre = analogRead(presion);
    P = map(niv_pre,0,1024, 0,115);
    Serial.println(P);
    Serial.println(R);
    if (P!= R)
    {
      estado = 11;
      dif_temp= millis()-t0;
      Serial.println(dif_temp);
      if (dif_temp > tiempo_limite_bomba)
      {
      estado = 12;
      }
    }
    if(P==R)
    {
      estado = 13;
    }
    break;
    
    case 11:
    Serial.println("estado 11");
    Serial.println("Retroalimentacion y ajuste");
    if (R-P >0)
    {
    pwm = pwm+1;
    }
    else {
    pwm = pwm-1;
    }
    analogWrite(pin_PWM_bomba, pwm);
    estado = 10;
    break;
    
    case 12:
    Serial.println("estado 12");
    Serial.println("Analizar bloqueo");
    voltaje_motor();
    
    if (voltaje_real_mot == valor_bloqueo)
    {
      i=4;
      Serial.println("Se detecto bloqueo");
      
    }
    else{
      i=5;
      Serial.println("Se detecto Fuga");
    }
    estado= 7;
    break;
    
    case 13:
    Serial.println("estado 13");
    Serial.println("Listo 2");
    if (S ==1)
    {
      estado =0;
    }
    else{
      estado = 14;
    }
    break;
    
    case 14:
    Serial.println("estado 14");
    Serial.println("desactivar bomba poco a poco");
    
    if (pwm >0)
    {
    pwm = pwm-1;
    }
    analogWrite(pin_PWM_bomba, pwm);
    if (m==1)
    { 
     tone(pin_buzz, 300,10); 
    }
    estado = 15;
    
    break;
    
    case 15:
    Serial.println("estado 15");
    Serial.println("Lectura de S");
    S=digitalRead(START);
    if (S ==1)
    {
      estado = 14;
    }
    else{
      estado =16;
    }
    break;
    
    case 16:
    Serial.println("estado 16");
    Serial.println("Modificacion en el aparato");
    noTone(pin_buzz);
    S=digitalRead(START);
    if (S==0)
    {
      estado = 17;
    }
    else
    {
      estado = 0;
    }
    break;
    case 17:
    Serial.println("estado 17");
    Serial.println("Seguir desactivando la bomba poco a poco");
    if (pwm >0)
    {
    pwm = pwm-1;
    }
    analogWrite(pin_PWM_bomba, pwm);
    estado = 16;
    break;
    case 18:
    Serial.println("estado 18");
    Serial.println("Comienza el tipo de terapia variable");
    break;
  }
}  

void voltaje_bat()
{
  volt=0.0;
  for (int j=1; j<= 100; j++)
  {
    //para aumentar la sensibilidad se usa el for
    val= analogRead(analog_in);
    volt= (float) val * 5.0/1024.0 + volt;
  }
  voltaje = (float) volt/100.0;
  voltaje_real = voltaje*((R1+R2)/R2);
  Serial.print(voltaje_real);
  Serial.println("voltios");
  Bat = (voltaje_real/volt_fuente_max)*100;
}

void voltaje_motor()
{
  volt_mot=0.0;
  for (int k=1; k<= 100; k++)
  {
    val_mot= analogRead(analog_in_mot);
    volt_mot= (float) val_mot * 5.0/1024.0 + volt_mot;
  }
  voltaje_mot = (float) volt_mot/100.0;
  voltaje_real_mot = voltaje_mot*((R3+R4)/R4);
  Serial.print(voltaje_real_mot);
  Serial.println("voltios");
}
