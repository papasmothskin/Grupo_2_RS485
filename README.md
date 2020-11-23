# Grupo_2_RS485

EasyEda: https://easyeda.com/editor#id=|9db0600eaf7b4b838ae15ca7e6e2a753|093fb406e05b4480a0a622ae852f6a99|9c0c428ce9c346ae9c715c8660fc8352

# Funcionamento

## Inicialização

Antes de inicializar o UART, faz-se a leitura do endereço do dispositivo. Foi decidido que depois da leitura ser feita não será mais usado

Depois, se a leitura for 0, é o dispositivo mestre. Neste caso, só precisa da transmissão ativa.

Se o endereço for diferente de 0, é um slave, por isso o modo de multiprocessamento é ligado. Para além disso, decidiu-se fazer a receção da informação por interupção em vez de espera continua por receção de nova informação.

## Transmissão de dados

O mestre só irá transimitir dados no caso de uma nova trasição dos botões. Ele guarda o estado lido do ciclo anterior e compara com a nova.

Caso esta seja diferente, ele inicia o envio de uma nova mensagem. Caso o endereço do slave seja o mesmo que o destinatário anterior, é apenas enviado um frame de dados com o valor do botão.

Em caso contrário, é enviado antes o frame de endereço com o valor do endereço do slave a receber a mensagem, seguido do data frame.

## Receção de dados

O slave está em modo MPCM na inicialização e quando a ultima mensagem não lhe era destinada.

Este funcionamento é inerente ao hardware. Sempre que o slave esteja com este modo ativo, só haverá receção de address frames, sendo os data frames ignorados.

Então, se o slave receber um address frame com o seu endereço, ele desliga o MPCM e ouve pela a informação no canal de comunicação. Os outros slaves vão ignorar estar mensagens de dados. Quando o mestre envia uma mensagem a um novo rementente, este volta envia um address frame com o endereço do novo dispositivo. O dispositivo que estava a ouvir anteriormente reconhece que as mensagens seguintes não são mais para ele e volta a ligar o MPCM.