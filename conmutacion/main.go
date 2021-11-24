package main

import (
	"encoding/json"
	"net/http"

	"github.com/gorilla/mux"
)

type Datos struct {
	Servomotor int  `json:"servomotor"`
	Led        bool `json:"led"`
	Suiche     bool `json:"suiche"`
}

var datos Datos = Datos{}

func leer(w http.ResponseWriter, r *http.Request) {
	json.NewEncoder(w).Encode(datos)
}

func escribir(w http.ResponseWriter, r *http.Request) {
	json.NewDecoder(r.Body).Decode(&datos)
	json.NewEncoder(w).Encode(datos)
}

func main() {

	r := mux.NewRouter()
	r.HandleFunc("/read", leer).Methods("GET")
	r.HandleFunc("/write", escribir).Methods("POST")
	http.ListenAndServe(":10000", r)
}
