/* This code is autogenerated.
You can find the source code here https://git.it.hs-heilbronn.de/jemaier/lorawan-akdt-hw
Or email the author at mpfeffer@stud.hs-heilbronn.de or gohbgl@gmail.com */
!function(){var __webpack_modules__={973:function(module){function BitReader(bytes,bitOffset){this.bytes=bytes,this.bitIndex=bitOffset}BitReader.prototype.read=function(bitLength){if(bitLength>32)throw new Error("Cannot read more than ".concat(32," bits. Requested: ").concat(bitLength," bits."));if(bitLength<=0)throw new Error("Must at least read one bit. Requested: ".concat(bitLength," bits."));for(var bitsLeft=bitLength,bytes=this.bytes,byteIndex=this.bitIndex>>>3,bitIndex=7&this.bitIndex,value=0;bitsLeft>0;){var bitsRead=Math.min(bitsLeft,8-bitIndex),chunk=bytes[byteIndex];chunk>>>=bitIndex,chunk&=~(-1<<bitsRead),value|=chunk<<=bitLength-bitsLeft,byteIndex+=bitIndex+bitsRead>>>3,bitIndex=bitIndex+bitsRead&7,bitsLeft-=bitsRead}return this.bitIndex=8*byteIndex+bitIndex,value>>>=0},module.exports={BitReader:BitReader}},545:function(module){module.exports={VERSION_BITS:8,MEASUREMENT_COUNT_BITS:8}},160:function(module){module.exports={fieldBits:function(field){var distinctValueCount=(field.range.max-field.range.min)/field.resolution+1;return Math.ceil(Math.log2(distinctValueCount))},fractionDigits:function(number){var resolutionStr=number.toString(),dotIndex=resolutionStr.lastIndexOf(".");return-1===dotIndex?0:resolutionStr.length-dotIndex-1},unparen:function(s){return s.length>=2&&"("===s[0]&&")"===s[s.length-1]?s.substr(1,s.length-2):s}}},907:function(module,__unused_webpack_exports,__webpack_require__){function _createForOfIteratorHelper(o,allowArrayLike){var it;if("undefined"==typeof Symbol||null==o[Symbol.iterator]){if(Array.isArray(o)||(it=function(o,minLen){if(!o)return;if("string"==typeof o)return _arrayLikeToArray(o,minLen);var n=Object.prototype.toString.call(o).slice(8,-1);"Object"===n&&o.constructor&&(n=o.constructor.name);if("Map"===n||"Set"===n)return Array.from(o);if("Arguments"===n||/^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(n))return _arrayLikeToArray(o,minLen)}(o))||allowArrayLike&&o&&"number"==typeof o.length){it&&(o=it);var i=0,F=function(){};return{s:F,n:function(){return i>=o.length?{done:!0}:{done:!1,value:o[i++]}},e:function(_e){throw _e},f:F}}throw new TypeError("Invalid attempt to iterate non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.")}var err,normalCompletion=!0,didErr=!1;return{s:function(){it=o[Symbol.iterator]()},n:function(){var step=it.next();return normalCompletion=step.done,step},e:function(_e2){didErr=!0,err=_e2},f:function(){try{normalCompletion||null==it.return||it.return()}finally{if(didErr)throw err}}}}function _arrayLikeToArray(arr,len){(null==len||len>arr.length)&&(len=arr.length);for(var i=0,arr2=new Array(len);i<len;i++)arr2[i]=arr[i];return arr2}var BitReader=__webpack_require__(973).BitReader,versions=__webpack_require__(52),constants=__webpack_require__(545),_require2=__webpack_require__(160),fieldBits=_require2.fieldBits,fractionDigits=_require2.fractionDigits;this.Decoder=function(bytes,_port){if(0===bytes.length)throw new Error("Empty message received.");var bitReader=new BitReader(bytes,0),version=bitReader.read(constants.VERSION_BITS),messageFormat=versions[version];if(!messageFormat)throw new Error("Unknown message version: ".concat(version));var _step,fields=messageFormat.fields,_iterator=_createForOfIteratorHelper(fields);try{for(_iterator.s();!(_step=_iterator.n()).done;){var _field=_step.value;_field.bits=fieldBits(_field),_field.fractionDigits=fractionDigits(_field.resolution)}}catch(err){_iterator.e(err)}finally{_iterator.f()}var preludeBits=constants.VERSION_BITS+constants.MEASUREMENT_COUNT_BITS,measurementBits=fields.reduce((function(sum,f){return sum+f.bits}),0),minBytes=Math.ceil((preludeBits+measurementBits)/8);if(bytes.length<minBytes)throw new Error("Not enough bytes received. Minimum: ".concat(messageFormat.minBytes," bytes, received: ").concat(bytes.length," bytes."));var measurementCount=bitReader.read(constants.MEASUREMENT_COUNT_BITS),expectedBytes=Math.ceil((preludeBits+measurementBits*measurementCount)/8);if(bytes.length!==expectedBytes)throw new Error("Expected message size for ".concat(measurementCount," measurements to be exactly ").concat(expectedBytes," bytes. Received: ").concat(bytes.length,"."));for(var decoded={version:version,measurements:[]},i=0;i<measurementCount;i++){var _step2,measurement={},_iterator2=_createForOfIteratorHelper(fields);try{for(_iterator2.s();!(_step2=_iterator2.n()).done;){var field=_step2.value,value=bitReader.read(field.bits)*field.resolution+field.range.min;measurement[field.property]=value.toFixed(field.fractionDigits)}}catch(err){_iterator2.e(err)}finally{_iterator2.f()}decoded.measurements.push(measurement)}return decoded},module.exports=this.Decoder},52:function(module){var versions={1:{number:1,fields:[{name:"Ground temperature",unit:"°C",range:{min:-20,max:85},resolution:.5,property:"temperatureGround"},{name:"Ground water content",unit:"%",range:{min:0,max:50},resolution:1,property:"moistureGround"},{name:"Illuminance",unit:"lx",range:{min:0,max:12e4},resolution:1,property:"light"},{name:"Air temperature",unit:"°C",range:{min:-40,max:80},resolution:.5,property:"temperatureAir"},{name:"Relative air humidity",unit:"%",range:{min:0,max:100},resolution:1,property:"moistureAir"},{name:"Barometric pressure",unit:"hPa",range:{min:300,max:1100},resolution:1,property:"pressure"},{name:"Battery charge level",unit:"%",range:{min:0,max:100},resolution:1,property:"battery"}]}};module.exports=versions}},__webpack_module_cache__={};!function __webpack_require__(moduleId){if(__webpack_module_cache__[moduleId])return __webpack_module_cache__[moduleId].exports;var module=__webpack_module_cache__[moduleId]={exports:{}};return __webpack_modules__[moduleId].call(module.exports,module,module.exports,__webpack_require__),module.exports}(907)}();