#include "../AST_Tipos.h"

UnidadTraduccion::UnidadTraduccion() {}

UnidadTraduccion::~UnidadTraduccion() {
	for ( Definicion* d : definiciones ) {
		delete d;
	}
}

TipoDato UnidadTraduccion::analizarTipo() {
	tablaSimbolos->agregaContexto ( "0_PROGRAMA" );
	for ( Definicion* def : definiciones ) {
		if ( def->analizarTipo( ) == T_ERROR ) {
			return T_ERROR;
		}
	}
	if(!tablaSimbolos->existeMain()){
        return T_ERROR;
	}
	return T_VACIO;
}

string UnidadTraduccion::toString() {
	stringstream ss;
	ss << "<PROGRAMA>" << endl;

	for ( Definicion* d : definiciones ) {
		ss << d->toString();
	}
	ss << "</PROGRAMA>" << endl;

	return ss.str();
}

string UnidadTraduccion::generarCodigo() {
	vector<DefinicionFuncion*> funciones;
	vector<DefinicionVariable*> variables;
	stringstream ss;

	for ( Definicion* d : definiciones ) {
		if ( DefinicionVariable* dv = dynamic_cast<DefinicionVariable*> ( d ) ) {
			variables.push_back ( dv );
		} else {
			funciones.push_back ( dynamic_cast<DefinicionFuncion*> ( d ) );
		}
	}

	ss << ".section .rodata" << endl;
	ss << endl;

	ss << ".data" << endl;
	for ( DefinicionVariable* dv : variables ) {
		for ( Asignacion* a : dv->asignaciones ) {
			ss << ".globl " << a->id->simbolo << endl;
			ss << ".align 4" << endl;
            #if __linux__
                ss << ".type " << a->id->simbolo << ", @object" << endl;
            #endif
			ss << a->id->simbolo << ": " << endl;
			ss << TABULADOR << ".long " << a->evaluar() << endl;
		}
	}
	ss << endl;

	ss << ".text" << endl;
	for ( DefinicionFuncion* df : funciones ) {
		if ( df->cuerpo != nullptr ) {
			ss << ".globl " << df->id->simbolo << endl;
            #if __linux__
                ss << ".type " << df->id->simbolo << ", @fuction" << endl;
            #endif
		}
	}
	ss << endl;

	for ( DefinicionFuncion* df : funciones ) {
		if ( df->cuerpo != nullptr ) {
			ss << df->generarCodigo();
		}
	}

	return ss.str();
}
