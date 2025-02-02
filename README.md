# webserv 🌐

## Descripción 📜
**webserv** es un proyecto en C++ que implementa un servidor HTTP básico, diseñado para manejar solicitudes web y servir contenido estático (archivos HTML, imágenes, etc.) a los clientes.
El servidor incluye soporte para CGI (Common Gateway Interface), lo que permite la ejecución de scripts de servidor dinámicos. 

Este proyecto fue diseñado para poner en práctica conceptos fundamentales de redes, manejo de solicitudes HTTP, configuración de servidores y manejo de archivos de configuración.

## Objetivo 🎯
El objetivo de **webserv** es construir un servidor HTTP funcional desde cero, implementando las siguientes características:

- **Manejo de solicitudes HTTP**: El servidor es capaz de recibir y procesar solicitudes GET, POST y HEAD.
- **Soporte para archivos estáticos**: El servidor puede servir archivos HTML, CSS, JavaScript e imágenes desde el sistema de archivos local.
- **Soporte para CGI**: El servidor puede ejecutar scripts dinámicos mediante CGI.
- **Configuración del servidor**: El comportamiento del servidor se configura mediante un archivo de configuración que define parámetros como el puerto de escucha, el directorio raíz, los límites de tamaño de los archivos y las rutas CGI.
- **Manejo de errores**: El servidor maneja errores de forma adecuada, respondiendo con los códigos de estado HTTP correspondientes (por ejemplo, 404 para archivos no encontrados).

## Características ⚙️
- **Manejo de solicitudes HTTP**: Procesa y responde solicitudes HTTP de manera eficiente.
- **CGI (Common Gateway Interface)**: Permite la ejecución de scripts dinámicos en el servidor.
- **Configuración personalizada**: Configuración flexible a través de un archivo `.conf`.
- **Manejo de errores HTTP**: Responde con los códigos de estado adecuados para situaciones como archivos no encontrados (404), servidor no disponible (500), etc.
- **Soporte para archivos estáticos**: Sirve archivos estáticos desde un directorio especificado.
  
## Instalación ⚒️

### Requisitos 📋
- **C++98**: El proyecto está diseñado para cumplir con el estándar C++98.
- **Make**: Se utiliza para la compilación del proyecto.
- **C++**: Compilador de C++.

### Pasos para la instalación 💻

1. **Clona el repositorio**:
    ```bash
    git clone https://github.com/tu_usuario/webserv.git
    ```

2. **Accede al directorio del proyecto**:
    ```bash
    cd webserv
    ```

3. **Compila el proyecto con Make**:
    ```bash
    make
    ```

    Esto compilará el proyecto y generará el ejecutable `webserv`.

4. **Ejecuta el servidor**:
    ```bash
    ./webserv [archivo_de_configuración]
    ```

   Sustituye `[archivo_de_configuración]` por el archivo de configuración que deseas usar.
Hay algunos ya hechos en la carpeta configs, podrías probar con:
```bash
./webserv configs/complete_config.conf
```
y luego entra en tu navegador y entra en la página:
```plain text
http://127.0.0.1:8080/
```
