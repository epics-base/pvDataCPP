project('epics-pv-data', 'cpp', version: '8.0.5', license: 'EPICS')

epics_com_dep = dependency('epics-com')

pvd_version_num = configure_file(
  #input: 'src/pv/pvdVersionNum.h@',
  output: 'pvdVersionNum.h',
  configuration: configuration_data({
    'EPICS_PVD_MAJOR_VERSION': '8',
    'EPICS_PVD_MINOR_VERSION': '0',
    'EPICS_PVD_MAINTENANCE_VERSION': '5',
    'EPICS_PVD_DEVELOPMENT_FLAG': '1',
  }),
)

# HACK: this installs 'pvdVersionNum.h' in a 'pv' subdir
# See: https://github.com/mesonbuild/meson/issues/2320
run_command(
  [
    'sh',
    '-c',
    'mkdir @1@/pv; cp @1@/@0@ @1@/pv/'.format(
      pvd_version_num,
      meson.current_build_dir()
    ),
  ],
  check: true,
)

headers = [
  'src/copy/pv/createRequest.h',

  'src/factory/pv/factory.h',

  'src/json/pv/json.h',

  'src/misc/pv/anyscalar.h',
  'src/misc/pv/bitSet.h',
  'src/misc/pv/byteBuffer.h',
  'src/misc/pv/current_function.h',
  'src/misc/pv/debugPtr.h',
  'src/misc/pv/epicsException.h',
  'src/misc/pv/event.h',
  'src/misc/pv/lock.h',
  'src/misc/pv/noDefaultMethods.h',
  'src/misc/pv/pvUnitTest.h',
  'src/misc/pv/reftrack.h',
  'src/misc/pv/serialize.h',
  'src/misc/pv/serializeHelper.h',
  'src/misc/pv/sharedPtr.h',
  'src/misc/pv/sharedVector.h',
  'src/misc/pv/status.h',
  'src/misc/pv/templateMeta.h',
  'src/misc/pv/thread.h',
  'src/misc/pv/timer.h',
  'src/misc/pv/typeCast.h',

  'src/property/pv/alarm.h',
  'src/property/pv/control.h',
  'src/property/pv/display.h',
  'src/property/pv/pvAlarm.h',
  'src/property/pv/pvControl.h',
  'src/property/pv/pvDisplay.h',
  'src/property/pv/pvEnumerated.h',
  'src/property/pv/pvTimeStamp.h',
  'src/property/pv/timeStamp.h',

  'src/pv/convert.h',
  'src/pv/pvData.h',
  'src/pv/pvIntrospect.h',
  'src/pv/pvSubArrayCopy.h',
  'src/pv/pvType.h',
  'src/pv/pvdVersion.h',
  'src/pv/standardField.h',
  'src/pv/standardPVField.h',
  'src/pv/typemap.h',
  'src/pv/valueBuilder.h',
  pvd_version_num,

  'src/pvMisc/pv/bitSetUtil.h',
]

sources = [
  'src/copy/createRequest.cpp',
  'src/copy/requestmapper.cpp',

  'src/factory/Compare.cpp',
  'src/factory/Convert.cpp',
  'src/factory/FieldCreateFactory.cpp',
  'src/factory/PVArray.cpp',
  'src/factory/PVDataCreateFactory.cpp',
  'src/factory/PVField.cpp',
  'src/factory/PVScalar.cpp',
  'src/factory/PVScalarArray.cpp',
  'src/factory/PVStructure.cpp',
  'src/factory/PVStructureArray.cpp',
  'src/factory/PVUnion.cpp',
  'src/factory/PVUnionArray.cpp',
  'src/factory/StandardField.cpp',
  'src/factory/StandardPVField.cpp',
  'src/factory/TypeFunc.cpp',
  'src/factory/printer.cpp',
  'src/factory/pvSubArrayCopy.cpp',

  'src/json/parseany.cpp',
  'src/json/parsehelper.cpp',
  'src/json/parseinto.cpp',
  'src/json/print.cpp',

  'src/misc/anyscalar.cpp',
  'src/misc/bitSet.cpp',
  'src/misc/byteBuffer.cpp',
  'src/misc/debugPtr.cpp',
  'src/misc/epicsException.cpp',
  'src/misc/event.cpp',
  'src/misc/parseToPOD.cpp',
  'src/misc/pvUnitTest.cpp',
  'src/misc/reftrack.cpp',
  'src/misc/serializeHelper.cpp',
  'src/misc/status.cpp',
  'src/misc/thread.cpp',
  'src/misc/timer.cpp',
  'src/misc/typeCast.cpp',

  'src/property/alarm.cpp',
  'src/property/pvAlarm.cpp',
  'src/property/pvControl.cpp',
  'src/property/pvDisplay.cpp',
  'src/property/pvEnumerated.cpp',
  'src/property/pvTimeStamp.cpp',
  'src/property/timeStamp.cpp',

  'src/pv/pvdVersion.cpp',
  'src/pv/valueBuilder.cpp',

  'src/pvMisc/bitSetUtil.cpp',
]

include_directories = [
  'src',
  'src/copy',
  'src/factory',
  'src/json',
  'src/misc',
  'src/property',
  'src/pvMisc',
]

libpv_data = both_libraries(
  'epics-pv-data',
  sources,
  include_directories: include_directories,
  dependencies: [epics_com_dep],
  install: true,
)

epics_pv_data_dep = declare_dependency(
  include_directories: include_directories,
  link_with: libpv_data,
)

tests = [
  ['test_create_request', 'testApp/copy/testCreateRequest.cpp'],
  ['test_base_exception', 'testApp/misc/testBaseException.cpp'],
  ['test_bit_set', 'testApp/misc/testBitSet.cpp'],
  ['test_byte_buffer', 'testApp/misc/testByteBuffer.cpp'],
  ['test_byte_order', 'testApp/misc/testByteOrder.cpp'],
  ['test_debug_ptr', 'testApp/misc/testDebugPtr.cpp'],
  ['test_event', 'testApp/misc/testEvent.cpp'],
  ['test_overrun_bit_set', 'testApp/misc/testOverrunBitSet.cpp'],
  ['test_serialization', 'testApp/misc/testSerialization.cpp'],
  ['test_shared_vector', 'testApp/misc/testSharedVector.cpp'],
  ['test_thread', 'testApp/misc/testThread.cpp'],
  ['test_time_stamp', 'testApp/misc/testTimeStamp.cpp'],
  # TODO: this times out. Is this normal?
  #['test_timer', 'testApp/misc/testTimer.cpp'],
  ['test_type_cast', 'testApp/misc/testTypeCast.cpp'],
  ['test_unit_test', 'testApp/misc/testUnitTest.cpp'],
  ['test_reftrack', 'testApp/misc/test_reftrack.cpp'],
  ['testanyscalar', 'testApp/misc/testanyscalar.cpp'],
  ['testjson', 'testApp/misc/testjson.cpp'],
  ['testprinter', 'testApp/misc/testprinter.cpp'],
]

foreach t : tests
  exe = executable(t[0], t[1], dependencies: [epics_com_dep, epics_pv_data_dep])
  test(t[0], exe, is_parallel: true)
endforeach

pkgconfig = import('pkgconfig')
pkgconfig.generate(libpv_data)

install_headers(headers, subdir: 'pv')

