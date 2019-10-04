/*******************************************************************************
 * Copyright (c) 2019, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/

/** @file nvdata.c
 * @brief This file implements common stuff to handle Non Volatile data.
 *
 * This module implements NvdataLoad(), a function to load all NV data of
 *  known common objects.
 * Also this module provides callback functions to store NV data of known
 *  objects when called by the EIP stack.
 */
#include "nvdata.h"

#include "trace.h"

/* Include headers of objects that need support for NV data here. */
#include "nvqos.h"

/** @brief Load NV data for all object classes
 *
 *  @return kEipStatusOk on success, kEipStatusError if failure for any object occurred
 *
 * This function loads the NV data for each object class that supports NV data from
 *  external storage. If any of the load routines fails then for that object class
 *  the current object instance values are written as new NV data. That should be
 *  the default data.
 *
 * The load routines should be of the form
 *    int Nv<ObjClassName>Load(<ObjectInstanceDataType> *p_obj_instance);
 *  and return (-1) on failure and (0) on success.
 */
EipStatus NvdataLoad(void) {
  EipStatus status = kEipStatusOk;
  int       rc;

  /* Load NV data for QoS object instance */
  rc = NvQosLoad(&g_qos);
  status |= rc;
  if (0 != rc) {
    rc = NvQosStore(&g_qos);
    status |= rc;
  }

  return status;
}

/** A PostSetCallback for QoS class to store NV attributes
*
* @param  instance  pointer to instance of QoS class
* @param  attribute pointer to attribute structure
* @param  service   the CIP service code of current request
*
* This function implements the PostSetCallback for the QoS class. The
* purpose of this function is to save the NV attributes of the QoS
* class instance to external storage.
*
* This application specific implementation chose to save all attributes
* at once using a single NvQosStore() call.
*/
EipStatus NvQosSetCallback
(
  CipInstance *const instance,
  CipAttributeStruct *const attribute,
  CipByte service
)
{
  EipStatus status = kEipStatusOk;

  if (0 != (kNvDataFunc & attribute->attribute_flags)) {
    OPENER_TRACE_INFO("NV data update: %s, i %" PRIu32 ", a %" PRIu16 "\n",
                      instance->cip_class->class_name,
                      instance->instance_number,
                      attribute->attribute_number);
    status = NvQosStore(&g_qos);
  }
  return status;
}
