import time

from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates
from pydantic import BaseModel


class SendData(BaseModel):
    value: float


last_value = None
last_time = None


app = FastAPI()
templates = Jinja2Templates(directory="templates")


@app.get("/")
async def home(request: Request):
    return templates.TemplateResponse(
        request,
        "index.html",
        context={"title": "ESP8266", "status": "Connected"},
    )


@app.post("/value")
async def set_value(data: SendData):
    global last_time, last_value
    last_value = data.value
    last_time = time.time()
    return "ok"


@app.get("/value")
async def current_value():
    return {"time": last_time, "value": last_value}
