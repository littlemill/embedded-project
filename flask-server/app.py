from flask import Flask, request

app = Flask(__name__)

status = None
@app.route('/handler', methods=['POST'])
def handler():
    global status
    status = (request.data).decode('utf-8')
    return ""

@app.route('/status', methods=['GET'])
def readStatus():
    global status
    return 'Status : {}'.format(status)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8090)


