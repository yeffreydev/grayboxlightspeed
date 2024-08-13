# instalar

Boost

1. descarga boost https://www.boost.org/
2. utilizar el ejecutador .bat (boost_1_75_0\bootstrap.bat)
3. segui el paso que indica al final (./b2)
4. agrega a las variables de entorno la ruta de boost. boost_1_75_0

## setup crow

https://crowcpp.org/master/getting_started/setup/windows/

1. descarga crow
2. generar header file only con python. (merge_all.py)
3. luego agregar al proyecto. crow_all.h.
4. luego configura vscode que se pueda descargar paquetes con n solution explorer, right click the name of your project then click Properties.
   Under vcpkg, set Use Vcpkg Manifest to Yes and Additional Options to --feature-flags="versions".
   Set Debug/Release and x64/x86.
   Run.

5. copiar el archiv json de crow a la carpeta root de proyecto.

## configurar proyecto con LightspeedTrader API

agregar la carpeta include a la configuracion de c/c++ en includePath. en propiedades > general > configuracion de c/c++ > includePath

## ejecutar

6. al hacer f5, se descargara los paquetes necesarios y se compilara.

## api service

PORT: 18080

### GET /positions

obtiene las posiciones de la cuenta.

### GET /order/<id>

obtiene la orden con el id.

### GET /position/<symbol>

obtiene la posicion de un simbolo.

### POST /order

parametros (symbol, entryPrice, qty, side("buy" o "sell));
crea una orden y retorna el id. {id:1}

### DELETE /order/<id>

cancela la orden con el id.
