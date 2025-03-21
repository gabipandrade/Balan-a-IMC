#include "HX711.h"                                        /*Biblioteca do HX711 (Amplificador operacional da balança) */
#include <Ultrasonic.h>                                   /*Biblioteca para o sensor ultrassônico;*/

const int LOADCELL_DOUT_PIN = 3;                          /*Declaração do pino DOUT da célula de carga*/
const int LOADCELL_SCK_PIN = 2;                           /*Declaração do pino SCK da célula de carga*/
const int pino_trigger = 4;                               /*Declaração do pino trigger do sensor ultrasônico*/
const int pino_echo = 5;                                  /*Declaração do pino echo do sensor ultrasônico*/

int i = 0;                                                /*Declaração da variável que armazenará a idade do usuário*/
char s = '\0';                                                   /*Declaração da variável que armazenará o sexo do usuário*/
int pare = 0;                                              /*Declaração da variável que auxiliará no controle de entradas*/
int denominador = 0;                                       /*Declaração da variável que armazenará a quantidade de vezes que a altura foi medida para efetuar o calculo da altura final*/
int fatorAtividade = 0;                                    /*O fator de atividade física determina o seu estilo de vida para o calculo do consumo calórico diário ideal*/
float altura, massa, IMC, alturaMedida;

Ultrasonic ultrasonic(pino_trigger, pino_echo);           /*Inicializa o sensor ultrassônico de acordo com os pinos declarados*/
HX711 balanca;                                            /*Inicializa a biblioteca dos sensores de carga e declara o nome atribuído à balança*/                   

void leitura(){
  Serial.println("Insira a sua idade: ");
  Serial.available();
  Serial.flush();
  Serial.end();
  Serial.begin(9600);

  while(pare == 0){
  if (Serial.available()){
    i = Serial.parseInt();
    delay(900);
    Serial.print("A sua idade eh: ");
    Serial.println(i);
    Serial.println("");
    Serial.flush();
    Serial.available();
    Serial.end();
    Serial.begin(9600);
    Serial.println("Digite 'f' para sexo feminino e 'm' para masculino: ");
    pare ++;
  }
}

  while(pare == 1){
  if (Serial.available()){
    s = Serial.read();
    Serial.print("O seu sexo eh: ");
    Serial.println(s);
    Serial.println("");
    Serial.flush();
    Serial.end();
    Serial.begin(9600);
    pare++;
    }
  }  
} 

void medidas(){
  delay(2000);
  massa = balanca.get_units(), 100;                                /*Calcula a massa do usuário através da média entre 100 medições para diluir o erro*/
  massa = massa * -1;                                              /*Multiplicador devido às celulas de carga terem sido instaladas da maneira inversa*/
  delay(2000);

  long microsec = ultrasonic.timing();  
  for(int i = 0; i < 100; i++){                                    /*O loop faz 100 medições da altura da pessoa e as armazena na variável altura*/
  alturaMedida = ultrasonic.convert(microsec, Ultrasonic::CM); 
  delay(60);
  if(alturaMedida > 0){                                            /*Desconsidera as alturas medidas como negativas (Caso o usuario se mova)*/
  altura = altura + alturaMedida;                                  /*Armazena as medidas positivas na variável altura*/
  denominador++;                                                   /*A variavel denominador será o denominador da fração que tirará a média das N medidas positivas*/
    }
}
  altura = altura / 100;                                           /*Transforma a medida em metros*/
  altura = altura / denominador;                                   /*Tira a média das N medições para diluir o erro*/
  altura = 1.877 - altura;  /*Modificador para calcular a altura entre a cabeça do usuario e o topo do sensor ultrassonico*/
  tone(6,349,400);                                                 /*Apita o Buzzer para indiciar o fim das medições*/
  
  Serial.print("A sua massa é: ");
  Serial.print(massa);
  Serial.println(" Kg");
  Serial.println("");
  delay(600);

  Serial.print("A sua altura é: ");
  Serial.print(altura);
  Serial.println(" m");
  Serial.println("");
  delay(600);
  pare++;
}

void calculo_IMC(){  
  double ccd;                                                               /*Declaração da variável que estipula o consumo calórico diário ideal para o usuário manter uma vida saudável*/
  double tmb;                                                               /*Declaração da variável que estipula a taxa metabólica basal, o mínimo de ingestão calorica diaria para o usuário manter as suas funções vitais*/
  double ida;                                                               /*Declaração da variável que estipula a quantidade recomendada de ingestão diária de agua para o usuário*/
  int hcm;                                                                  /*Declaração da variavel que armazena a altura do usuário em centímetros, dado necessário para alguns dos cálculos*/
  hcm = altura * 100;
       
    IMC = massa / (altura * altura);                                        /*Calcula o IMC pela fórmula usual*/    
    Serial.print("O valor do seu IMC eh: ");  
    delay(500);
    Serial.println(IMC);
    Serial.println("");
    delay(5000);
    
    if(IMC < 18.5){
      Serial.println("De acordo com o seu IMC medido, em comparação com os dados da OMS, você está abaixo do peso ideal.");
    }
    if(18.5 <= IMC && IMC < 24.9){
      Serial.println("De acordo com o seu IMC medido, em comparação com os dados da OMS, você está com o peso ideal.");
    }
    if(25.0 <= IMC && IMC< 29.9){
      Serial.println("De acordo com o seu IMC medido, em comparação com os dados da OMS, você está com pré-obesidade(Sobrepeso).");
    }
    if(30.0 <= IMC && IMC < 34.9){
      Serial.println("De acordo com o seu IMC medido, em comparação com os dados da OMS, você está com obesidade de grau 1 (Leve).");
    }
    if(35.0 <= IMC && IMC < 39.9){
      Serial.println("De acordo com o seu IMC medido, em comparação com os dados da OMS, você está com obesidade de grau 2 (Severa).");
    }
    if(IMC > 40){
      Serial.println("De acordo com o seu IMC medido, em comparação com os dados da OMS, você está com obesidade de grau 3 (Mórbida).");
    }
    Serial.println("");
    delay(5000);
    
    ida = 0.035 * massa;                                                         /*Especialistas recomendam a ingestão de 35ml de água por cada Kg*/
    Serial.print("Recomenda-se que você beba ");
    Serial.print(ida);
    Serial.println(" litros de água por dia!");
    Serial.println("");
    delay(5000);
    
    switch(s){
        case 'f':                                                             /*Cálculos da TMB no caso da usuária ser uma mulher*/
        tmb =  655 + (9.6 * massa) + (1.8 * hcm) - (4.7 * i);
        Serial.print("Sua taxa metabolica basal é: ");
        Serial.print(tmb);
        Serial.println(" Calorias!");
        Serial.println("");
        break;
        
        case 'm':                                                             /*Cálculos da TMB no caso do usuário ser um homem*/
        tmb = 66 + (13.8 * massa) + (5 * hcm) - (6.8 * i);
        Serial.print("Sua taxa metabolica basal é: ");
        Serial.print(tmb);
        Serial.println(" Calorias!");
        Serial.println("");
        break;
    }
    delay(5000);

  Serial.println("Agora precisaremos determinar o seu estilo de vida para o cálculo da sua quantidade de ingestão calórica diária");
  Serial.println(" para a manutenção de uma vida saudável!");
  delay(5000);
  Serial.println("Digite 1 caso o seu estilo de vida envolva atividades leves, como:");
  Serial.println("  - Trabalhar sentado;");
  Serial.println("  - Caminhadas de até 1 hora;");
  Serial.println("  - cuidar de crianças;");
  delay(2000);
  Serial.println("Digite 2 caso o seu estilo de vida envolva atividades moderadas, como:");
  Serial.println("  - Trabalhar envolvendo caminhadas ou estar de pé;");
  Serial.println("  - Fazer atividades físicas como natação, corrida ou dança;");
  delay(2000);
  Serial.println("Digite 3 caso o seu estilo de vida envolva atividades intensas, como:");
  Serial.println("  - Trabalhadores braçais;");
  Serial.println("  - Mais de duas horas diárias de esportes de alta demanda;");
  delay(2000);
  Serial.flush();
  Serial.end();
  Serial.begin(9600);

  while(pare == 3){                                        /*Aguarda a resposta do usuário sobre seu estilo de vida e atribui à variável fatorAtividade*/
  if (Serial.available()){
    fatorAtividade = Serial.read();
    delay(1500);
    pare ++;
    }
  }
  Serial.println("");
  fatorAtividade = fatorAtividade - 48;
  Serial.print("A opção selecionada foi: ");
  Serial.println(fatorAtividade, DEC);
  Serial.println("");
  delay(5000);
  
  switch(fatorAtividade){
    case 1:
    ccd = tmb * 1.55;
    Serial.print("A sua quantidade de ingestão calórica diária recomendada para o seu estilo de vida eh: ");
    Serial.print(ccd);
    Serial.println(" Calorias!");
    break;

    case 2:
    ccd = tmb * 1.84;
    Serial.print("A sua quantidade de ingestão calórica diária recomendada para o seu estilo de vida eh: ");
    Serial.print(ccd);
    Serial.println(" Calorias!");
    break;

    case 3:
    ccd = tmb * 2.2;
    Serial.print("A sua quantidade de ingestão calórica diária recomendada para o seu estilo de vida eh: ");
    Serial.print(ccd);
    Serial.println(" Calorias!");
    break;   
  }
  Serial.println("");
  delay(5000);
}

void setup() {
  Serial.begin(9600);                                     /*Inicializa o monitor serial*/
  Serial.println("Prototipo da melhor balança ja feita pelo ser humano!!! :) ;)");
  delay(800);
  Serial.println("Por favor, insira usando o teclado os dados pedidos: ");
  Serial.println("");
  delay(800);
  balanca.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  balanca.power_up();
  balanca.set_scale();                     
  balanca.tare();
  balanca.set_scale(21600.00);
  pinMode(6, OUTPUT);                                     /*Declaração da porta digital do buzzer*/
  delay(900);
}


void loop(){
  leitura();
     
  if(pare == 2){
  Serial.println("Obrigado pelos seus dados! Assim poderemos realizar uma avaliação médica mais precisa!");
  delay(1500);
  Serial.println("Agora retire todo o peso extra presente em você e posicione-se debaixo do laser!");
  delay(1500);
  Serial.println("A balança apitará quando detectar a sua presença e fará todas as medições necessárias!");
  delay(1500);
  Serial.println("Só desca da mesma quando escutar o segundo apito!");
  Serial.println("");
  delay(8000);  
  tone(6,349,400);                                /*Apita o Buzzer para indiciar o início das medições*/ 
  medidas();
  }

  if (pare == 3){
  delay(1000);
  Serial.println("Agora faremos o cálculo do seu IMC e compararemos com as faixas de peso ideais de acordo com as suas medidas");
  delay(1800);
  Serial.println("Tenha em mente que as medidas feitas pela balança não substituem um exame médico!");
  delay(1800);
  Serial.println("Portanto, caso alguma medida te preocupe procure acompanhamento profissional!");
  Serial.println("");
  delay(3000);
  calculo_IMC();
  }

  if(pare == 4){
  Serial.println("Obrigado por utilizar a nossa balança!");
  Serial.println("Lembre-se, nenhum dos calculos efetuados pela mesma substituirá um acompanhamento profissional!");
  delay(1500);
  exit (0);
  }
}
